package main

import (
	"fmt"
	"log"
	"strings"
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
			debugMessage("Application problems:", r.renderProblems())
		}
	}
}

func (r *Reviewer) printErrorAndExit(err error) {
	log.Fatalf("Error reviewing issue: %s\n", err.Error())
}

func (r *Reviewer) renderProblems() string {
	var problemStrings []string

	for _, err := range r.application.Problems {
		problemStrings = append(problemStrings, fmt.Sprintf("- %s", err.Error()))
	}

	return strings.Join(problemStrings, "\n")
}
