package main

import (
	"encoding/json"
	"fmt"
	"log"
	"os"

	"github.com/google/go-github/v60/github"
)

type TestApplicationSet struct {
	name        string
	body        string
	issueTitle  string
	statusLabel string
	isClosed    bool
}

var testIssue *github.Issue

var testApplications = []TestApplicationSet{
	{
		name:        "project",
		body:        "project",
		issueTitle:  "Application for TestDB",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "team",
		body:        "team",
		issueTitle:  "Application for AcmeCo",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "event",
		body:        "event",
		issueTitle:  "Application for FooConf",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "no-body",
		body:        "",
		issueTitle:  "",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "no-responses",
		body:        "no-responses",
		issueTitle:  "Application for [project name]",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "invalid-1",
		body:        "invalid-1",
		issueTitle:  "Application for [project name]",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "character-test",
		body:        "character-test",
		issueTitle:  "Application for TestDB",
		isClosed:    false,
		statusLabel: "",
	},
	{
		name:        "status-approved",
		body:        "project",
		issueTitle:  "Application for TestDB",
		isClosed:    false,
		statusLabel: "approved",
	},
	{
		name:        "status-reviewing",
		body:        "project",
		issueTitle:  "Application for TestDB",
		isClosed:    false,
		statusLabel: "reviewing",
	},
	{
		name:        "status-invalid",
		body:        "project",
		issueTitle:  "Application for TestDB",
		isClosed:    false,
		statusLabel: "invalid",
	},
	{
		name:        "closed",
		body:        "project",
		issueTitle:  "Application for TestDB",
		isClosed:    true,
		statusLabel: "",
	},
	{
		name:        "closed-approved",
		body:        "project",
		issueTitle:  "Application for TestDB",
		isClosed:    true,
		statusLabel: "approved",
	},
}

func isTesting() bool {
	return testIssue != nil || os.Getenv("TEST") == "true"
}

func debugMessage(message string, data ...interface{}) {
	if isTesting() {
		fmt.Printf("[DEBUG] %s\n", message)

		for _, entry := range data {
			fmt.Println(entry)
		}
	}
}

func botMessage(message string) {
	if isTesting() {
		fmt.Printf("[BOT] %s\n", message)
	}
}

func findTestApplication(testName string) (*TestApplicationSet, error) {
	for _, app := range testApplications {
		if app.name == testName {
			return &app, nil
		}
	}

	return nil, fmt.Errorf("cannot find test application named '%s'", testName)
}

func loadIssue(issue interface{}) error {
	data, err := os.ReadFile("./script/mock-issue.json")

	if err != nil {
		return fmt.Errorf("unable to read file: %v", err)
	}

	if err := json.Unmarshal(data, issue); err != nil {
		return fmt.Errorf("unable to unmarshal JSON: %v", err)
	}

	return nil
}

func setIssueProperties(application *TestApplicationSet, issue *github.Issue) error {
	issue.Title = &application.issueTitle

	if application.body != "" {
		issueBodyPath := fmt.Sprintf("./script/mock-issue-bodies/%s.md", application.body)
		issueBody, err := os.ReadFile(issueBodyPath)
		if err != nil {
			return fmt.Errorf("unable to read the file: %v", err)
		}
		bodyString := string(issueBody)
		issue.Body = &bodyString
	}

	if application.isClosed {
		state := "closed"
		issue.State = &state
	}

	if application.statusLabel != "" {
		label := fmt.Sprintf("status: %s", application.statusLabel)
		issue.Labels = append(issue.Labels, &github.Label{Name: &label})
	}

	return nil
}

func setTestApplication(testName string) {
	application, err := findTestApplication(testName)
	if err != nil {
		log.Fatalf(err.Error())
	}

	debugMessage(fmt.Sprintf("Testing application named '%s'", testName))

	var issue github.Issue
	if err := loadIssue(&issue); err != nil {
		log.Fatalf(err.Error())
	}

	if err := setIssueProperties(application, &issue); err != nil {
		log.Fatalf(err.Error())
	}

	if err := os.Setenv("APPROVER_ID", "123"); err != nil {
		log.Fatalf("Failed to set environment variable: %s", err)
	}
	if err := os.Setenv("APPROVER_USERNAME", "wendyappleseed"); err != nil {
		log.Fatalf("Failed to set environment variable: %s", err)
	}

	testIssue = &issue
}

func errSliceEqual(a, b []error) bool {
	if len(a) != len(b) {
		return false
	}

	countA := make(map[string]int)
	countB := make(map[string]int)

	for _, err := range a {
		countA[err.Error()]++
	}

	for _, err := range b {
		countB[err.Error()]++
	}

	if len(countA) != len(countB) {
		return false
	}

	for k, v := range countA {
		if countB[k] != v {
			return false
		}
	}

	return true
}
