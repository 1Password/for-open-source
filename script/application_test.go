package main

import (
	"fmt"
	"os"
	"testing"
)

func setupTestDir(targetDir string) (cleanupFunc func(), err error) {
	originalDir, err := os.Getwd()
	if err != nil {
		return nil, fmt.Errorf("failed to get current directory: %s", err)
	}

	if err := os.Chdir(targetDir); err != nil {
		return nil, fmt.Errorf("failed to change working directory: %s", err)
	}

	return func() {
		if err := os.Chdir(originalDir); err != nil {
			fmt.Printf("Failed to change back to original directory: %s\n", err)
		}
	}, nil
}

func errNoProjectName(sectionTitle string) error {
	return fmt.Errorf("**%s** is missing project name", sectionTitle)
}

func errIncomplete(sectionTitle string) error {
	return fmt.Errorf("**%s** was not completed for application", sectionTitle)
}

func errEmpty(sectionTitle string) error {
	return fmt.Errorf("**%s** is empty", sectionTitle)
}

func errMustBeChecked(sectionTitle string) error {
	return fmt.Errorf("**%s** must be checked", sectionTitle)
}

func errInvalidAccountURL(sectionTitle string) error {
	return fmt.Errorf("**%s** is invalid 1Password account URL", sectionTitle)
}

func errContainsEmoji(sectionTitle string) error {
	return fmt.Errorf("**%s** cannot contain emoji characters", sectionTitle)
}

func errParsingNumber(sectionTitle string) error {
	return fmt.Errorf("**%s** could not be parsed into a number", sectionTitle)
}

func errInvalidURL(sectionTitle string) error {
	return fmt.Errorf("**%s** is an invalid URL", sectionTitle)
}

func TestApplication_Parse(t *testing.T) {
	cleanup, err := setupTestDir("../")
	if err != nil {
		t.Fatalf(err.Error())
	}

	defer cleanup()

	testCases := []struct {
		name             string
		expectedValid    bool
		expectedProblems []error
	}{
		{
			name:          "project",
			expectedValid: true,
		},
		{
			name:          "team",
			expectedValid: true,
		},
		{
			name:          "event",
			expectedValid: true,
		},
		{
			name:          "character-test",
			expectedValid: true,
		},
		{
			name:          "no-body",
			expectedValid: false,
			expectedProblems: []error{
				errIncomplete("Account URL"),
				errIncomplete("Non-commercial confirmation"),
				errIncomplete("Project name"),
				errIncomplete("Short description"),
				errIncomplete("Number of team members/core contributors"),
				errIncomplete("Homepage URL"),
				errIncomplete("License type"),
				errIncomplete("License URL"),
				errIncomplete("Age confirmation"),
				errIncomplete("Name"),
				errIncomplete("Email"),
				errIncomplete("Project role"),
			},
		},
		{
			name:          "no-responses",
			expectedValid: false,
			expectedProblems: []error{
				errNoProjectName("Application title"),
				errEmpty("Account URL"),
				errMustBeChecked("Non-commercial confirmation"),
				errEmpty("Project name"),
				errEmpty("Short description"),
				errEmpty("Number of team members/core contributors"),
				errEmpty("Homepage URL"),
				errEmpty("License type"),
				errEmpty("License URL"),
				errMustBeChecked("Age confirmation"),
				errEmpty("Name"),
				errEmpty("Email"),
				errEmpty("Project role"),
			},
		},
		{
			name:          "invalid-1",
			expectedValid: false,
			expectedProblems: []error{
				errNoProjectName("Application title"),
				errInvalidAccountURL("Account URL"),
				errMustBeChecked("Non-commercial confirmation"),
				errContainsEmoji("Project name"),
				errParsingNumber("Number of team members/core contributors"),
				errInvalidURL("Homepage URL"),
			},
		},
	}

	for _, tt := range testCases {
		t.Run(tt.name, func(t *testing.T) {
			application := Application{}

			setTestApplication(tt.name)
			application.Parse(testIssue)

			if application.IsValid() != tt.expectedValid {
				if tt.expectedValid {
					t.Errorf("Test issue '%s' is invalid, expected valid", tt.name)
				} else {
					t.Errorf("Test issue '%s' is valid, expected invalid", tt.name)
				}
			}

			if !tt.expectedValid && !errSliceEqual(application.Problems, tt.expectedProblems) {
				t.Errorf("Expected problems %v, got %v", tt.expectedProblems, application.Problems)
			}
		})
	}
}

func TestApplication_FileName(t *testing.T) {
	testCases := []struct {
		issueNumber      int
		projectName      string
		expectedFilename string
	}{
		{123, "Test project", "123-test-project.json"},
		{456, "My_Team", "456-my_team.json"},
		{789, "Event: Re/Act?", "789-event-react.json"},
		{101, "Project--With---Dashes", "101-project-with-dashes.json"},
	}

	for _, tt := range testCases {
		t.Run(tt.expectedFilename, func(t *testing.T) {
			app := Application{
				IssueNumber: tt.issueNumber,
				Project:     Project{Name: tt.projectName},
			}

			filename := app.FileName()
			if filename != tt.expectedFilename {
				t.Errorf("FileName() is %s, expected %v", filename, tt.expectedFilename)
			}
		})
	}
}

func TestApplication_SetApprover(t *testing.T) {
	cleanup, err := setupTestDir("../")
	if err != nil {
		t.Fatalf(err.Error())
	}

	defer cleanup()

	setTestApplication("project")
	app := Application{}

	app.SetApprover()

	if app.ApproverId != 123 {
		t.Errorf("SetApprover() set ApproverId to %d, expected 123", app.ApproverId)
	}

	if app.ApproverUsername != "wendyappleseed" {
		t.Errorf("SetApprover() set ApproverUsername to %s, expected wendyappleseed", app.ApproverUsername)
	}
}
