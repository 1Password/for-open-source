package main

import (
	"log"
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

	if isTestingIssue() {
		if r.application.IsValid() {
			debugMessage("Application has no problems")
		} else {
			debugMessage("Application problems:", r.application.RenderProblems())
		}
	}
}

func (r *Reviewer) printErrorAndExit(err error) {
	log.Fatalf("Error reviewing issue: %s\n", err.Error())
}
