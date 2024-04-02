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
	if isTesting() {
		debugMessage("Skipping GitHub Init")
		g.Issue = testIssue
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

func (g *GitHub) CreateIssueComment(message string) error {
	if isTesting() {
		botMessage(message)
		return nil
	}

	_, _, err := g.Client.Issues.CreateComment(
		g.Context,
		g.RepoOwner,
		g.RepoName,
		*g.Issue.Number,
		&github.IssueComment{
			Body: github.String(message),
		},
	)

	return err
}

func (g *GitHub) IssueHasLabel(label string) bool {
	for _, entry := range g.Issue.Labels {
		if label == *entry.Name {
			return true
		}
	}

	return false
}

func (g *GitHub) AddIssueLabel(label string) error {
	if isTesting() {
		debugMessage(fmt.Sprintf("Adding label \"%s\"", label))
		return nil
	}

	_, _, err := g.Client.Issues.AddLabelsToIssue(
		g.Context,
		g.RepoOwner,
		g.RepoName,
		*g.Issue.Number,
		[]string{label},
	)

	return err
}

func (g *GitHub) RemoveIssueLabel(label string) error {
	if isTesting() {
		debugMessage(fmt.Sprintf("Removing label \"%s\"", label))
		return nil
	}

	_, err := g.Client.Issues.RemoveLabelForIssue(
		g.Context,
		g.RepoOwner,
		g.RepoName,
		*g.Issue.Number,
		label,
	)

	return err
}

func (g *GitHub) CloseIssue() error {
	if isTesting() {
		debugMessage("Closing issue")
		return nil
	}

	_, _, err := g.Client.Issues.Edit(
		g.Context,
		g.RepoOwner,
		g.RepoName,
		*g.Issue.Number,
		&github.IssueRequest{
			State: github.String("closed"),
		},
	)

	return err
}

func (g *GitHub) CommitNewFile(filePath string, content []byte, message string) error {
	commitBranch := "main"

	if isTesting() {
		debugMessage(
			fmt.Sprintf(
				"Commiting the following contents to a file located at \"%s\" with the commit message \"%s\":",
				filePath, message,
			),
			string(content),
		)

		return nil
	}

	_, _, err := g.Client.Repositories.CreateFile(
		g.Context,
		g.RepoOwner,
		g.RepoName,
		filePath,
		&github.RepositoryContentFileOptions{
			Content: content,
			Message: &message,
			Branch:  &commitBranch,
		},
	)

	return err
}
