package main

import (
	"fmt"
	"log"

	"github.com/getsentry/sentry-go"
)

type Status int

const (
	Approved Status = iota
	Reviewing
	Invalid
	New
)

type Reviewer struct {
	gitHub      *GitHub
	application *Application
}

func (r *Reviewer) Review() {
	if err := r.gitHub.Init(); err != nil {
		r.logErrorAndExit("could not initialize GitHub client", err)
	}

	if err := r.gitHub.InitIssue(); err != nil {
		r.logErrorAndExit("could not initialize GitHub issue", err)
	}

	r.application.Parse(r.gitHub.Issue)

	status := r.getStatus()
	isClosed := *r.gitHub.Issue.State == "closed"
	issueAuthor := *r.gitHub.Issue.User.Login

	r.updateLabels(status, isClosed)
	r.createComment(status, isClosed, issueAuthor)
}

func (r *Reviewer) getStatus() Status {
	if r.gitHub.IssueHasLabel(LabelStatusApproved) {
		return Approved
	} else if r.gitHub.IssueHasLabel(LabelStatusReviewing) {
		return Reviewing
	} else if r.gitHub.IssueHasLabel(LabelStatusInvalid) {
		return Invalid
	} else {
		return New
	}
}

func (r *Reviewer) createComment(status Status, isClosed bool, issueAuthor string) {
	title := ""
	body := ""

	applicationData := fmt.Sprintf("<details>\n<summary>Application data...</summary>\n\n```json\n%s\n```\n</details>", r.application.GetData())
	applicationFilePath := fmt.Sprintf("https://github.com/1Password/1password-teams-open-source/blob/main/data/%s", r.application.FileName())
	contactSnippet := "If you have any questions or this is an error, contact us at [opensource@1password.com](mailto:opensource@1password.com)."
	approvedBody := fmt.Sprintf("@%s this application has already been approved and changes will not be reviewed. If you would like to modify the details of your application, submit a pull request against the stored [application data](%s). %s", issueAuthor, applicationFilePath, contactSnippet)
	closedBody := fmt.Sprintf("@%s this application is closed and changes will not be reviewed. %s", issueAuthor, contactSnippet)

	// If the issue is closed, let the user know that they can't make changes.
	// If the issue was closed because it got approved, let them know how they can
	// modify their application details after the fact.
	if isClosed {
		if status == Approved {
			body = approvedBody
		} else {
			body = closedBody
		}
		// This scanerio should never occur, as an approved issue should
		// immediately be closed, but let's cover all bases.
	} else if status == Approved {
		body = approvedBody
	} else if r.application.IsValid() {
		if status == Reviewing {
			title = "### 👍 Application still valid"
			body = fmt.Sprintf("\n\n%s\n\n@%s we’ve run our automated pre-checks and your updated application is still valid.", applicationData, issueAuthor)
		} else {
			title = "### ✅ Your application is valid"
			body = fmt.Sprintf("\n\n%s\n\n@%s thanks for applying! Our automated pre-checks have determined your application is valid. Next step: our team will review your application and may have follow-up questions. You can still make changes to your application and it’ll be re-evaluated.", applicationData, issueAuthor)
		}
	} else {
		title = "### ❌ Your application is invalid"
		body = fmt.Sprintf("\n\n%s\n\n@%s our automated pre-checks have detected the following problems:\n\n%s\n\nUpdate this issue to correct these problems and we’ll automatically re-evaluate your application. %s", applicationData, issueAuthor, r.application.RenderProblems(), contactSnippet)
	}

	r.gitHub.CreateIssueComment(fmt.Sprintf("%s%s", title, body))
}

func (r *Reviewer) updateLabels(status Status, isClosed bool) {
	if status == Approved || isClosed {
		return
	}

	if r.application.IsValid() {
		if status == Invalid {
			if err := r.gitHub.RemoveIssueLabel(LabelStatusInvalid); err != nil {
				r.logErrorAndExit(
					fmt.Sprintf("could not remove issue label '%s'", LabelStatusInvalid),
					err,
				)
			}
		}

		if status != Reviewing {
			if err := r.gitHub.AddIssueLabel(LabelStatusReviewing); err != nil {
				r.logErrorAndExit(
					fmt.Sprintf("could not add issue label '%s'", LabelStatusReviewing),
					err,
				)
			}
		}
	} else {
		if status != Invalid {
			if err := r.gitHub.AddIssueLabel(LabelStatusInvalid); err != nil {
				r.logErrorAndExit(
					fmt.Sprintf("could not add issue label '%s'", LabelStatusInvalid),
					err,
				)
			}
		}

		if status == Reviewing {
			if err := r.gitHub.RemoveIssueLabel(LabelStatusReviewing); err != nil {
				r.logErrorAndExit(
					fmt.Sprintf("could not remove issue label '%s'", LabelStatusReviewing),
					err,
				)
			}
		}
	}
}

func (r *Reviewer) logErrorAndExit(message string, err error) {
	sentry.CaptureException(err)
	log.Fatalf("Error reviewing issue: %s: %s\n", message, err.Error())
}
