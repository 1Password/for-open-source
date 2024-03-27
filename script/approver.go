package main

import (
	"errors"
	"fmt"
	"log"
	"path/filepath"
	"strings"
)

type Approver struct {
	gitHub      GitHub
	application Application
}

func (a *Approver) Approve() {
	a.gitHub = GitHub{}
	a.application = Application{}

	if err := a.application.SetApprover(); err != nil {
		a.printErrorAndExit(err)
	}

	if err := a.gitHub.Init(); err != nil {
		a.printErrorAndExit(err)
	}

	if *a.gitHub.Issue.State == "closed" {
		a.printErrorAndExit(errors.New("script run on closed issue"))
	}

	if !a.gitHub.IssueHasLabel(LabelStatusApproved) {
		a.printErrorAndExit(
			fmt.Errorf("script run on issue that does not have required '%s' label", LabelStatusApproved),
		)
	}

	a.application.Parse(a.gitHub.Issue)

	if !a.application.IsValid() {
		a.printErrorAndExit(
			fmt.Errorf("script run on issue with invalid application data:\n\n%s", a.renderProblems()),
		)
	}

	// The reviewer may remove this label themselves, but
	// let's double check and remove it if they haven't
	if a.gitHub.IssueHasLabel(LabelStatusReviewing) {
		if err := a.gitHub.RemoveIssueLabel(LabelStatusReviewing); err != nil {
			a.printErrorAndExit(
				fmt.Errorf("could not remove issue label '%s': %s", LabelStatusReviewing, err.Error()),
			)
		}
	}

	if err := a.gitHub.CommitNewFile(
		filepath.Join("data", a.application.FileName()),
		a.application.GetData(),
		fmt.Sprintf("Added \"%s\" to program", a.application.Project.Name),
	); err != nil {
		a.printErrorAndExit(
			fmt.Errorf("could not create commit: %s", err.Error()),
		)
	}

	if err := a.gitHub.CreateIssueComment(a.getApprovalMessage()); err != nil {
		a.printErrorAndExit(
			fmt.Errorf("could not create issue comment: %s", err.Error()),
		)
	}

	if err := a.gitHub.CloseIssue(); err != nil {
		a.printErrorAndExit(
			fmt.Errorf("could not close issue: %s", err.Error()),
		)
	}
}

func (a *Approver) printErrorAndExit(err error) {
	log.Fatalf("Error approving application: %s\n", err.Error())
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
