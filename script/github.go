package main

import (
	"context"
	"errors"
	"fmt"
	"strconv"

	"github.com/google/go-github/v60/github"
	"golang.org/x/oauth2"
)

type GitHub struct {
	RepoOwner string
	RepoName  string
	Context   context.Context
	Client    *github.Client
	Issue     *github.Issue
}

func (g *GitHub) Init() error {
	if isTestingIssue() {
		debugMessage("Skipping GitHub Init")
		return nil
	}

	token, err := getEnv("OP_BOT_PAT")
	if err != nil {
		return err
	}

	issueNumberValue, err := getEnv("ISSUE_NUMBER")
	if err != nil {
		return err
	}

	issueNumber, err := strconv.Atoi(issueNumberValue)
	if err != nil {
		return errors.New("could not parse variable ISSUE_NUMBER")
	}

	repoOwnerValue, err := getEnv("REPOSITORY_OWNER")
	if err != nil {
		return err
	}
	g.RepoOwner = repoOwnerValue

	repoNameValue, err := getEnv("REPOSITORY_NAME")
	if err != nil {
		return err
	}
	g.RepoName = repoNameValue

	g.Context = context.Background()
	ts := oauth2.StaticTokenSource(
		&oauth2.Token{AccessToken: token},
	)
	tc := oauth2.NewClient(g.Context, ts)

	g.Client = github.NewClient(tc)

	issue, response, err := g.Client.Issues.Get(g.Context, g.RepoOwner, g.RepoName, issueNumber)
	if response.StatusCode != 200 {
		return fmt.Errorf("error retrieving issue: %s", response.Status)
	} else if err != nil {
		return err
	}

	g.Issue = issue

	return nil
}
