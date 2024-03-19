package main

import (
	"encoding/json"
	"fmt"
	"log"
	"os"

	"github.com/google/go-github/v60/github"
)

var testIssueName string

func isTestingIssue() bool {
	return testIssueName != ""
}

func debugMessage(message string, data ...interface{}) {
	if isTestingIssue() {
		fmt.Printf("[DEBUG] %s\n", message)

		for _, entry := range data {
			fmt.Println(entry)
		}
	}
}

func getTestIssue() github.Issue {
	data, err := os.ReadFile(fmt.Sprintf("./script/test-issues/%s.json", testIssueName))
	if err != nil {
		log.Fatalf("Unable to read the file: %v", err)
	}

	var issue github.Issue
	err = json.Unmarshal(data, &issue)

	if err != nil {
		log.Fatalf("Unable to unmarshal JSON: %v", err)
	}

	return issue
}
