package main

import (
	"fmt"
	"log"
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
	fmt.Println(r.GitHub.Issue)
}

func (r *Reviewer) PrintErrorAndExit(err error) {
	log.Fatalf("Error reviewing issue: %s\n", err.Error())
}
