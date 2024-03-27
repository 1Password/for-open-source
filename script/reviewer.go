package main

import (
	"fmt"
	"log"
)

type Status int

const (
	Closed Status = iota
	Approved
	Reviewing
	Invalid
	New
)

type Reviewer struct {
	gitHub      GitHub
	application Application
}

func (r *Reviewer) Review() {
	r.gitHub = GitHub{}
	r.application = Application{}

	if err := r.gitHub.Init(); err != nil {
		r.printErrorAndExit(err)
	}

	r.application.Parse(r.gitHub.Issue)

	status := r.getStatus()
	r.updateLabels(status)
	r.createComment(status)
}

func (r *Reviewer) getStatus() Status {
	if *r.gitHub.Issue.State == "closed" {
		return Closed
	} else if r.gitHub.IssueHasLabel(LabelStatusApproved) {
		return Approved
	} else if r.gitHub.IssueHasLabel(LabelStatusReviewing) {
		return Reviewing
	} else if r.gitHub.IssueHasLabel(LabelStatusInvalid) {
		return Invalid
	} else {
		return New
	}
}

func (r *Reviewer) createComment(status Status) {
	title := ""
	body := ""
	details := fmt.Sprintf("<details>\n<summary>Application data...</summary>\n\n```json\n%s\n```\n</details>", r.application.GetData())
	// TODO: replace FILE_NAME with Application.FileName once available
	dataPath := fmt.Sprintf("https://github.com/1Password/1password-teams-open-source/blob/main/data/%s", "FILE_NAME")

	if status == Closed {
		body = "This application is closed and changes will not be reviewed. If this is an error, contact us at [opensource@1password.com](mailto:opensource@1password.com)."
	} else if status == Approved {
		body = fmt.Sprintf("This application has already been approved and changes will not be reviewed. If you would like to modify the details of your application, submit a pull request against the stored [application data](%s). If this is an error, contact us at [opensource@1password.com](mailto:opensource@1password.com).", dataPath)
	} else if status == Reviewing && r.application.IsValid() {
		title = "### 👍 Application still valid"
		body = fmt.Sprintf("\n\n%s\n\nWe’ve evaluated your updated application and it is still valid.", details)
	} else if r.application.IsValid() {
		title = "### ✅ Your application is valid"
		body = fmt.Sprintf("\n\n%s\n\nThanks for applying! Next step: our team will review your application and may have follow-up questions. You can still make changes to your application and it’ll be re-evaluated.", details)
	} else {
		title = "### ❌ Your application is invalid"
		body = fmt.Sprintf("\n\n%s\n\nThe following issues need to be addressed:\n\n%s", details, r.application.RenderProblems())
	}

	r.gitHub.CreateIssueComment(fmt.Sprintf("%s%s", title, body))
}

func (r *Reviewer) updateLabels(status Status) {
	if status == Approved || status == Closed {
		return
	}

	if status == Invalid && r.application.IsValid() {
		if err := r.gitHub.RemoveIssueLabel(LabelStatusInvalid); err != nil {
			r.printErrorAndExit(
				fmt.Errorf("could not remove issue label '%s': %s", LabelStatusInvalid, err.Error()),
			)
		}
	}

	if r.application.IsValid() {
		if status != Reviewing {
			if err := r.gitHub.AddIssueLabel(LabelStatusReviewing); err != nil {
				r.printErrorAndExit(
					fmt.Errorf("could not add issue label '%s': %s", LabelStatusReviewing, err.Error()),
				)
			}
		}
	} else {
		if status != Invalid {
			if err := r.gitHub.AddIssueLabel(LabelStatusInvalid); err != nil {
				r.printErrorAndExit(
					fmt.Errorf("could not add issue label '%s': %s", LabelStatusInvalid, err.Error()),
				)
			}
		}

		if status == Reviewing {
			if err := r.gitHub.RemoveIssueLabel(LabelStatusReviewing); err != nil {
				r.printErrorAndExit(
					fmt.Errorf("could not remove issue label '%s': %s", LabelStatusReviewing, err.Error()),
				)
			}
		}
	}
}

func (r *Reviewer) printErrorAndExit(err error) {
	log.Fatalf("Error reviewing issue: %s\n", err.Error())
}
