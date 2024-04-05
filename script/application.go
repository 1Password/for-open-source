package main

import (
	"encoding/json"
	"fmt"
	"log"
	"regexp"
	"strconv"
	"strings"
	"time"

	"github.com/getsentry/sentry-go"
	"github.com/google/go-github/v60/github"
)

const (
	LabelStatusApproved  = "status: approved"
	LabelStatusInvalid   = "status: invalid"
	LabelStatusReviewing = "status: reviewing"
)

type Project struct {
	Name         string `json:"name"`
	Description  string `json:"description"`
	Contributors int    `json:"contributors"`
	HomeURL      string `json:"home_url"`
	RepoURL      string `json:"repo_url,omitempty"`
	LicenseType  string `json:"license_type,omitempty"`
	LicenseURL   string `json:"license_url,omitempty"`
	IsEvent      bool   `json:"is_event"`
	IsTeam       bool   `json:"is_team"`
}

type Applicant struct {
	Name  string `json:"name"`
	Email string `json:"email"`
	Role  string `json:"role"`
	ID    int64  `json:"id"`
}

type Application struct {
	validator Validator         `json:"-"`
	sections  map[string]string `json:"-"`
	Problems  []error           `json:"-"`

	Account          string    `json:"account"`
	Project          Project   `json:"project"`
	Applicant        Applicant `json:"applicant"`
	CanContact       bool      `json:"can_contact"`
	ApproverId       int       `json:"approver_id,omitempty"`
	ApproverUsername string    `json:"-"`
	IssueNumber      int       `json:"issue_number"`
	CreatedAt        time.Time `json:"created_at"`
}

func (a *Application) Parse(issue *github.Issue) {
	a.validator = Validator{}

	if strings.Contains(*issue.Title, "[project name]") {
		a.validator.AddError("Application title", *issue.Title, "is missing project name")
	}

	a.sections = a.extractSections(*issue.Body)

	if isTesting() {
		data, err := json.MarshalIndent(a.sections, "", "\t")
		if err != nil {
			sentry.CaptureException(err)
			log.Fatalf("Could not marshal Sections input data: %s", err.Error())
		}

		debugMessage("Parsed input data:", string(data))
	}

	a.CreatedAt = issue.CreatedAt.Time
	a.IssueNumber = *issue.Number
	a.Account = a.stringSection("Account URL", true, ParseAccountURL)
	a.boolSection("Non-commercial confirmation", true, ParseCheckbox, IsChecked)

	a.Project.IsTeam = a.boolSection("Team application", false, ParseCheckbox)
	a.Project.IsEvent = a.boolSection("Event application", false, ParseCheckbox)

	isProject := !a.Project.IsTeam && !a.Project.IsEvent

	a.Project.Name = a.stringSection("Project name", true, ParsePlainString)
	a.Project.Description = a.stringSection("Short description", true, ParsePlainString)
	a.Project.Contributors = a.intSection("Number of team members/core contributors", true, ParsePlainString)
	a.Project.HomeURL = a.stringSection("Homepage URL", true, IsURL)
	a.Project.RepoURL = a.stringSection("Repository URL", false, IsURL)
	a.Project.LicenseType = a.stringSection("License type", isProject, ParsePlainString)
	a.Project.LicenseURL = a.stringSection("License URL", isProject, IsURL)
	a.boolSection("Age confirmation", isProject, ParseCheckbox, When(isProject, IsChecked))

	a.Applicant.Name = a.stringSection("Name", true, ParsePlainString)
	a.Applicant.Email = a.stringSection("Email", true, IsEmail)
	a.Applicant.Role = a.stringSection("Project role", true)
	a.Applicant.ID = *issue.User.ID

	a.stringSection("Profile or website", false, IsURL)
	a.stringSection("Additional comments", false)

	a.CanContact = a.boolSection("Can we contact you?", false, ParseCheckbox)

	if isTesting() {
		debugMessage("Application data:", string(a.GetData()))
	}

	for _, err := range a.validator.Errors {
		a.Problems = append(a.Problems, fmt.Errorf(err.Error()))
	}
}

func (a *Application) IsValid() bool {
	return len(a.Problems) == 0
}

func (a *Application) RenderProblems() string {
	var problemStrings []string

	for _, err := range a.Problems {
		problemStrings = append(problemStrings, fmt.Sprintf("- %s", err.Error()))
	}

	return strings.Join(problemStrings, "\n")
}

func (a *Application) GetData() []byte {
	data, err := json.MarshalIndent(a, "", "\t")
	if err != nil {
		sentry.CaptureException(err)
		log.Fatalf("Could not marshal Application data: %s", err.Error())
	}

	return data
}

// FileName takes application issue number and project name and turn it
// into a file path. This will always be unique because it is relying on
// GitHub's issue numbers
// e.g. 782-foo.json
func (a *Application) FileName() string {
	filename := fmt.Sprintf("%s-%s.json",
		strconv.FormatInt(int64(a.IssueNumber), 10),
		strings.ToLower(a.Project.Name),
	)

	filename = strings.ReplaceAll(strings.ToLower(filename), " ", "-")
	filename = regexp.MustCompile(`[^\w.-]`).ReplaceAllString(filename, "")
	filename = regexp.MustCompile(`-+`).ReplaceAllString(filename, "-")

	return filename
}

func (a *Application) SetApprover() error {
	approverIdValue, err := getEnv("APPROVER_ID")
	if err != nil {
		return err
	}

	approverId, err := strconv.Atoi(approverIdValue)
	if err != nil {
		return err
	}

	approverUsername, err := getEnv("APPROVER_USERNAME")
	if err != nil {
		return err
	}

	a.ApproverId = approverId
	a.ApproverUsername = approverUsername

	return nil
}

// Take the Markdown-format body of an issue and break it down by section header
// and the content directly below it. We can reasonably expect the correct format
// here if someone files an issue using the application template, but it will also
// gracefully handle when this format is not present. Note that this will only
// create an entry when there is content to be added; in other words, a section
// header without any content will not be added.
func (a *Application) extractSections(body string) map[string]string {
	sections := make(map[string]string)

	lines := strings.Split(body, "\n")
	var currentHeader string
	contentBuilder := strings.Builder{}

	// For each line of the body content, it can either be a section's
	// header or the content associated with that section's header.
	for _, line := range lines {
		trimmedLine := strings.TrimSpace(line)

		// If we're in a section and the content doesn't start with
		// a header marker, append it to our content builder
		if !strings.HasPrefix(trimmedLine, "### ") {
			if currentHeader == "" {
				continue
			}

			contentBuilder.WriteString(line + "\n")
			continue
		}

		// The content has a header marker, so create a new
		// section entry and prepare the content builder
		if currentHeader != "" && contentBuilder.Len() > 0 {
			sections[currentHeader] = strings.TrimSpace(contentBuilder.String())
			contentBuilder.Reset()
		}

		currentHeader = strings.TrimSpace(trimmedLine[4:])
	}

	// Once the loop has completed check if there's a
	// trailing section needing to be closed
	if currentHeader != "" && contentBuilder.Len() > 0 {
		sections[currentHeader] = strings.TrimSpace(contentBuilder.String())
	}

	return sections
}

func (a *Application) stringSection(sectionName string, required bool, callbacks ...ValidatorCallback) string {
	value, exists := a.sections[sectionName]
	_, value, _ = ParseInput(value)

	// If the section is required, apply the presence validator if the entry
	// exists, early fail validation if it doesn't exist. If the section is
	// not required and there is no content to work with, don't try to run
	// additional validations.
	if required {
		if exists {
			callbacks = append([]ValidatorCallback{IsPresent}, callbacks...)
		} else {
			a.validator.AddError(sectionName, value, "was not completed for application")
			return value
		}
	} else if !exists || value == "" {
		return value
	}

	for _, callback := range callbacks {
		pass, newValue, message := callback(value)
		value = newValue

		if !pass {
			a.validator.AddError(sectionName, value, message)
			break
		}
	}

	return value
}

func (a *Application) intSection(sectionName string, required bool, callbacks ...ValidatorCallback) int {
	value := a.stringSection(sectionName, required, callbacks...)

	// Don't bother proceeding if there's already an error parsing the string
	if a.validator.HasError(sectionName) || value == "" {
		return 0
	}

	pass, number, message := ParseNumber(value)
	if !pass {
		a.validator.AddError(sectionName, fmt.Sprintf("%d", number), message)
		return 0
	}

	return number
}

func (a *Application) boolSection(sectionName string, required bool, callbacks ...ValidatorCallback) bool {
	value := a.stringSection(sectionName, required, callbacks...)

	// Don't bother proceeding if there's already an error parsing the string
	if a.validator.HasError(sectionName) || value == "" {
		return false
	}

	pass, boolean, message := ParseBool(value)
	if !pass {
		a.validator.AddError(sectionName, fmt.Sprintf("%t", boolean), message)
		return false
	}

	return boolean
}
