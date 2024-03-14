package main

import (
	"fmt"
	"os"

	"github.com/google/go-github/github"
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
	fmt.Printf("Error reviewing issue: %s\n", err.Error())
	os.Exit(1)
}
