package main

import (
	"encoding/json"
	"fmt"
	"log"
	"strings"
	"time"

	"github.com/google/go-github/v60/github"
)

type Project struct {
	Name         string `json:"name"`
	Description  string `json:"description"`
	Contributors int    `json:"contributors"`
	HomeUrl      string `json:"home_url"`
	RepoUrl      string `json:"repo_url,omitempty"`
	LicenseType  string `json:"license_type,omitempty"`
	LicenseUrl   string `json:"license_url,omitempty"`
	IsEvent      bool   `json:"is_event"`
	IsTeam       bool   `json:"is_team"`
}

type Applicant struct {
	Name  string `json:"name"`
	Email string `json:"email"`
	Role  string `json:"role"`
	Id    int64  `json:"id"`
}

type Application struct {
	validator Validator         `json:"-"`
	sections  map[string]string `json:"-"`
	Problems  []error           `json:"-"`

	Account     string    `json:"account"`
	Project     Project   `json:"project"`
	Applicant   Applicant `json:"applicant"`
	CanContact  bool      `json:"can_contact"`
	ApproverId  int       `json:"approver_id,omitempty"`
	IssueNumber int       `json:"issue_number"`
	CreatedAt   time.Time `json:"created_at"`
}

func (a *Application) Parse(issue github.Issue) {
	a.validator = Validator{}

	if strings.Contains(*issue.Title, "[project name]") {
		a.validator.AddError("Application title", *issue.Title, "is missing project name")
	}

	a.sections = a.extractSections(*issue.Body)

	if isTestingIssue() {
		data, err := json.MarshalIndent(a.sections, "", "\t")
		if err != nil {
			log.Fatalf("Could not marshal Sections input data: %s", err.Error())
		}

		debugMessage("Parsed input data:", string(data))
	}

	a.CreatedAt = issue.CreatedAt.Time
	a.IssueNumber = *issue.Number
	a.Account = a.stringSection("Account URL", IsPresent, ParseAccountUrl)
	a.boolSection("Non-commercial confirmation", IsPresent, ParseCheckbox, IsChecked)

	a.Project.IsTeam = a.boolSection("Team application", ParseCheckbox)
	a.Project.IsEvent = a.boolSection("Event application", ParseCheckbox)

	isProject := !a.Project.IsTeam && !a.Project.IsEvent

	a.Project.Name = a.stringSection("Project name", IsPresent, ParsePlainString)
	a.Project.Description = a.stringSection("Short description", IsPresent, ParsePlainString)
	a.Project.Contributors = a.intSection("Number of team members/core contributors", IsPresent, ParsePlainString)
	a.Project.HomeUrl = a.stringSection("Homepage URL", IsPresent, IsUrl)
	a.Project.RepoUrl = a.stringSection("Repository URL", IsUrl)
	a.Project.LicenseType = a.stringSection("License type", When(isProject, IsPresent), ParsePlainString)
	a.Project.LicenseUrl = a.stringSection("License URL", When(isProject, IsPresent), IsUrl)
	a.boolSection("Age confirmation", When(isProject, IsPresent), ParseCheckbox, When(isProject, IsChecked))

	a.Applicant.Name = a.stringSection("Name", IsPresent, ParsePlainString)
	a.Applicant.Email = a.stringSection("Email", IsPresent, IsEmail)
	a.Applicant.Role = a.stringSection("Project role", IsPresent)
	a.Applicant.Id = *issue.User.ID

	a.stringSection("Profile or website", IsUrl)
	a.stringSection("Additional comments")

	a.CanContact = a.boolSection("Can we contact you?", ParseCheckbox)

	if isTestingIssue() {
		debugMessage("Application data:", a.GetData())
	}

	for _, err := range a.validator.Errors {
		a.Problems = append(a.Problems, fmt.Errorf(err.Error()))
	}
}

func (a *Application) IsValid() bool {
	return len(a.Problems) == 0
}

func (a *Application) GetData() string {
	data, err := json.MarshalIndent(a, "", "\t")
	if err != nil {
		log.Fatalf("Could not marshal Application data: %s", err.Error())
	}

	return string(data)
}

func (a *Application) extractSections(body string) map[string]string {
	sections := make(map[string]string)

	lines := strings.Split(body, "\n")
	var currentHeader string
	contentBuilder := strings.Builder{}

	for _, line := range lines {
		trimmedLine := strings.TrimSpace(line)
		if strings.HasPrefix(trimmedLine, "### ") {
			if currentHeader != "" {
				sections[currentHeader] = strings.TrimSpace(contentBuilder.String())
				contentBuilder.Reset()
			}
			currentHeader = strings.TrimSpace(trimmedLine[4:])
		} else if currentHeader != "" {
			contentBuilder.WriteString(line + "\n")
		}
	}

	if currentHeader != "" {
		sections[currentHeader] = strings.TrimSpace(contentBuilder.String())
	}

	return sections
}

func (a *Application) stringSection(sectionName string, callbacks ...ValidatorCallback) string {
	value, exists := a.sections[sectionName]

	if !exists {
		a.validator.AddError(sectionName, value, "was not completed for application")
		return value
	}

	// everything gets passed through ParseInput first
	callbacks = append([]ValidatorCallback{ParseInput}, callbacks...)

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

func (a *Application) intSection(sectionName string, callbacks ...ValidatorCallback) int {
	value := a.stringSection(sectionName, callbacks...)

	// don't bother proceeding if there's already an error parsing the string
	if a.validator.HasError(sectionName) {
		return 0
	}

	pass, number, message := ParseNumber(value)
	if !pass {
		a.validator.AddError(sectionName, fmt.Sprintf("%d", number), message)
		return 0
	}

	return number
}

func (a *Application) boolSection(sectionName string, callbacks ...ValidatorCallback) bool {
	value := a.stringSection(sectionName, callbacks...)

	// don't bother proceeding if there's already an error parsing the string
	if a.validator.HasError(sectionName) {
		return false
	}

	pass, boolean, message := ParseBool(value)
	if !pass {
		a.validator.AddError(sectionName, fmt.Sprintf("%t", boolean), message)
		return false
	}

	return boolean
}
