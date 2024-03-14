package main

import (
	"context"
	"errors"
	"fmt"
	"os"
	"strconv"

	"github.com/google/go-github/github"
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
		testMessage("Skipping GitHub initialization")
		return nil
	}

	token, err := g.GetEnv("OP_BOT_PAT")
	if err != nil {
		return err
	}

	issueNumberValue, err := g.GetEnv("ISSUE_NUMBER")
	if err != nil {
		return err
	}

	issueNumber, err := strconv.Atoi(issueNumberValue)
	if err != nil {
		return errors.New("could not parse variable ISSUE_NUMBER")
	}

	repoOwnerValue, err := g.GetEnv("REPOSITORY_OWNER")
	if err != nil {
		return err
	}
	g.RepoOwner = repoOwnerValue

	repoNameValue, err := g.GetEnv("REPOSITORY_NAME")
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

func (g *GitHub) GetEnv(key string) (string, error) {
	value := os.Getenv(key)

	if value == "" {
		return "", fmt.Errorf("missing variable %s", key)
	}

	return value, nil
}
