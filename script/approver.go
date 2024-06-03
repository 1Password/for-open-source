package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/getsentry/sentry-go"
)

type Approver struct {
	gitHub      *GitHub
	application *Application
}

func (a *Approver) Approve() {
	if err := a.application.SetApprover(); err != nil {
		a.logErrorAndExit("could not set application approver", err)
	}

	if err := a.gitHub.Init(); err != nil {
		a.logErrorAndExit("could not initialize GitHub client", err)
	}

	if err := a.gitHub.InitIssue(); err != nil {
		a.logErrorAndExit("could not initialize GitHub issue", err)
	}

	if *a.gitHub.Issue.State == "closed" {
		a.logErrorAndExit(
			"script run on closed issue",
			errors.New(*a.gitHub.Issue.State),
		)
	}

	if !a.gitHub.IssueHasLabel(LabelStatusApproved) {
		a.logErrorAndExit(
			fmt.Sprintf("script run on issue that does not have required '%s' label", LabelStatusApproved),
			fmt.Errorf("issue has labels %v", a.gitHub.Issue.Labels),
		)
	}

	a.application.Parse(a.gitHub.Issue)

	if !a.application.IsValid() {
		a.logErrorAndExit(
			"script run on issue with invalid application data",
			errors.New(a.renderProblems()),
		)
	}

	// The reviewer may remove this label themselves, but
	// let's double check and remove it if they haven't
	if a.gitHub.IssueHasLabel(LabelStatusReviewing) {
		if err := a.gitHub.RemoveIssueLabel(LabelStatusReviewing); err != nil {
			a.logErrorAndExit(
				fmt.Sprintf("could not remove issue label '%s'", LabelStatusReviewing),
				err,
			)
		}
	}

	if err := a.gitHub.CommitNewFile(
		filepath.Join("data", a.application.FileName()),
		a.application.GetData(),
		fmt.Sprintf("Added \"%s\" to program", a.application.Project.Name),
	); err != nil {
		a.logErrorAndExit(
			"could not create commit",
			err,
		)
	}

	if err := a.gitHub.CreateIssueComment(a.getApprovalMessage()); err != nil {
		a.logErrorAndExit(
			"could not create issue comment",
			err,
		)
	}

	if err := a.gitHub.CloseIssue(); err != nil {
		a.logErrorAndExit(
			"could not close issue",
			err,
		)
	}

	a.createSentryEvent()
}

func (a *Approver) logErrorAndExit(message string, err error) {
	sentry.CaptureException(err)
	log.Fatalf("Error approving issue: %s: %s\n", message, err.Error())
}

func (a *Approver) renderProblems() string {
	var problemStrings []string

	for _, err := range a.application.Problems {
		problemStrings = append(problemStrings, fmt.Sprintf("- %s", err.Error()))
	}

	return strings.Join(problemStrings, "\n")
}

func (a *Approver) getApprovalMessage() string {
	return strings.TrimSpace(fmt.Sprintf(`### 🎉 Your application has been approved

Congratulations, @%s has approved your application! A promotion will be applied to your 1Password account shortly.

To lower the risk of lockout, [assign at least one other person to help with account recovery](https://support.1password.com/team-recovery-plan/) in case access for a particular team member is ever lost. You may add additional core contributors as you see fit.

Finally, we’d love to hear more about your experience using 1Password in your development workflows! Feel free to join us over on the [1Password Developers Slack](https://join.slack.com/t/1password-devs/shared_invite/zt-15k6lhima-GRb5Ga~fo7mjS9xPzDaF2A) workspace.

Welcome to the program and happy coding! 🧑‍💻`, a.application.ApproverUsername))
}

func (a *Approver) createSentryEvent() {
	var appData map[string]interface{}
	err := json.Unmarshal(a.application.GetData(), &appData)
	if err != nil {
		sentry.CaptureException(err)
		log.Fatal(err)
	}

	var applicationType string
	if a.application.Project.IsTeam {
		applicationType = "team"
	} else if a.application.Project.IsEvent {
		applicationType = "event"
	} else {
		applicationType = "project"
	}

	tags := map[string]string{
		"project_name":     a.application.Project.Name,
		"homepage_url":     a.application.Project.HomeURL,
		"application_type": applicationType,
		"team_size":        strconv.Itoa(a.application.Project.Contributors),
		"issue_number":     strconv.Itoa(a.application.IssueNumber),
		"approved_by":      fmt.Sprintf("@%s", a.application.ApproverUsername),
		"can_contact":      strconv.FormatBool(a.application.CanContact),
	}

	sentry.CaptureEvent(&sentry.Event{
		Message: fmt.Sprintf("Application approved for \"%s\"", a.application.Project.Name),
		Level:   sentry.LevelInfo,
		Extra:   appData,
		Tags:    tags,
	})
}
