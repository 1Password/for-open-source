package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"path/filepath"
	"strings"

	"github.com/getsentry/sentry-go"
)

type Approver struct {
	gitHub      GitHub
	application Application
}

func (a *Approver) Approve() {
	a.gitHub = GitHub{}
	a.application = Application{}

	if err := a.application.SetApprover(); err != nil {
		a.logErrorAndExit("could not set application approver", err)
	}

	if err := a.gitHub.Init(); err != nil {
		a.logErrorAndExit("could not initialize GitHub client", err)
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

	var appData map[string]interface{}
	err := json.Unmarshal(a.application.GetData(), &appData)
	if err != nil {
		log.Fatal(err)
	}
	sentry.CaptureEvent(&sentry.Event{
		Message: "Application approved",
		Level:   sentry.LevelInfo,
		Extra:   appData,
	})
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
