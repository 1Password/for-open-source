package main

import (
	"fmt"
	"log"

	"github.com/google/go-github/v60/github"
)

type Reviewer struct {
	GitHub GitHub
}

func (r *Reviewer) Review() {
	r.GitHub = GitHub{}

	if err := r.GitHub.Init(); err != nil {
		r.PrintErrorAndExit(err)
	}

	// TODO: parse and validate the issue's body contents
	fmt.Println(r.GetIssue())
}

func (r *Reviewer) GetIssue() github.Issue {
	if isTestingIssue() {
		return getTestIssue()
	} else {
		return *r.GitHub.Issue
	}
}

func (r *Reviewer) PrintErrorAndExit(err error) {
	log.Fatalf("Error reviewing issue: %s\n", err.Error())
}
