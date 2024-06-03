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
	Client    *github.Client
	Issue     *github.Issue
}

func (g *GitHub) Init() error {
	if isTesting() {
		debugMessage("Skipping GitHub Init")
		return nil
	}

	token, err := getEnv("OP_BOT_PAT")
	if err != nil {
		return err
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

	ts := oauth2.StaticTokenSource(
		&oauth2.Token{AccessToken: token},
	)
	tc := oauth2.NewClient(context.Background(), ts)

	g.Client = github.NewClient(tc)

	return nil
}

func (g *GitHub) InitIssue() error {
	if isTesting() {
		debugMessage("Assigning test issue")
		g.Issue = testIssue
		return nil
	}

	issueNumberValue, err := getEnv("ISSUE_NUMBER")
	if err != nil {
		return err
	}

	issueNumber, err := strconv.Atoi(issueNumberValue)
	if err != nil {
		return errors.New("could not parse variable ISSUE_NUMBER")
	}

	issue, response, err := g.Client.Issues.Get(context.Background(), g.RepoOwner, g.RepoName, issueNumber)
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
		context.Background(),
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
		context.Background(),
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
		context.Background(),
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
		context.Background(),
		g.RepoOwner,
		g.RepoName,
		*g.Issue.Number,
		&github.IssueRequest{
			State: github.String("closed"),
		},
	)

	return err
}

func (g *GitHub) commitFile(filePath string, content []byte, message string, update bool) error {
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

	opts := &github.RepositoryContentFileOptions{
		Content: content,
		Message: &message,
		Branch:  &commitBranch,
	}

	var err error
	if update {
		fileContent, _, _, err := g.Client.Repositories.GetContents(
			context.Background(),
			g.RepoOwner,
			g.RepoName,
			filePath,
			&github.RepositoryContentGetOptions{Ref: commitBranch},
		)

		if err != nil {
			return err
		}

		sha := fileContent.GetSHA()
		opts.SHA = &sha

		_, _, err = g.Client.Repositories.UpdateFile(
			context.Background(),
			g.RepoOwner,
			g.RepoName,
			filePath,
			opts,
		)
	} else {
		_, _, err = g.Client.Repositories.CreateFile(
			context.Background(),
			g.RepoOwner,
			g.RepoName,
			filePath,
			opts,
		)
	}

	return err
}

func (g *GitHub) CommitNewFile(filePath string, content []byte, message string) error {
	return g.commitFile(filePath, content, message, false)
}

func (g *GitHub) UpdateFile(filePath string, content []byte, message string) error {
	return g.commitFile(filePath, content, message, true)
}
