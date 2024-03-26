package main

import (
	"fmt"
	"os"
	"testing"
)

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

func TestApplication(t *testing.T) {
	originalDir, err := os.Getwd()
	if err != nil {
		t.Fatalf("Failed to get current directory: %s", err)
	}

	defer func() {
		if err := os.Chdir(originalDir); err != nil {
			t.Fatalf("Failed to change back to original directory: %s", err)
		}
	}()

	if err := os.Chdir("../"); err != nil {
		t.Fatalf("Failed to change working directory: %s", err)
	}

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
			name:          "empty-body",
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
			name:          "examples-1",
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

			if tt.expectedValid {
				testIssueName = fmt.Sprintf("valid-%s", tt.name)
			} else {
				testIssueName = fmt.Sprintf("invalid-%s", tt.name)
			}

			application.Parse(getTestIssue())

			if application.IsValid() != tt.expectedValid {
				if tt.expectedValid {
					t.Errorf("Test issue '%s' is invalid, expected valid", testIssueName)
				} else {
					t.Errorf("Test issue '%s' is valid, expected invalid", testIssueName)
				}
			}

			if !tt.expectedValid && !errSliceEqual(application.Problems, tt.expectedProblems) {
				t.Errorf("Expected problems %v, got %v", tt.expectedProblems, application.Problems)
			}
		})
	}
}
