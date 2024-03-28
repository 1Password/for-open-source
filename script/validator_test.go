package main

import (
	"testing"
)

type validationTestCase struct {
	input         string
	expectedPass  bool
	expectedValue string
	expectedError string
}

func runValidationTests(t *testing.T, testCases []validationTestCase, f ValidatorCallback, funcName string) {
	for _, tc := range testCases {
		pass, value, _ := f(tc.input)
		if pass != tc.expectedPass {
			t.Errorf("%s(%s) passing is %v, expected %v", funcName, tc.input, pass, tc.expectedPass)
		}

		if value != tc.expectedValue {
			t.Errorf("%s(%s) got new value %v, expected %v", funcName, tc.input, value, tc.expectedValue)
		}
	}
}

func TestParseInput(t *testing.T) {
	testCases := []validationTestCase{
		{"", true, "", ""},
		{"_No response_", true, "", ""},
		{"None", true, "", ""},
		{"hello", true, "hello", ""},
		{"Testing <b>formatting</b> and <a href=\"#\">link</a> stripping", true, "Testing formatting and link stripping", ""},
	}
	runValidationTests(t, testCases, ParseInput, "ParseInput")
}

func TestParsePlainString(t *testing.T) {
	testCases := []validationTestCase{
		{"", true, "", ""},
		{"Hello world", true, "Hello world", ""},
		{"👋 howdy", false, "👋 howdy", "cannot contain emoji characters"},
	}
	runValidationTests(t, testCases, ParsePlainString, "ParsePlainString")
}

func TestParseAccountUrl(t *testing.T) {
	testCases := []validationTestCase{
		{"myteam.1password.com", true, "myteam.1password.com", ""},
	}
	runValidationTests(t, testCases, ParseAccountURL, "ParseAccountUrl")
}

func TestParseCheckbox(t *testing.T) {
	testCases := []validationTestCase{
		{"[x]", true, "true", ""},
		{"[X]", true, "true", ""},
		{"[]", true, "false", ""},
		{"[ ]", true, "false", ""},
		{"- [x] Howdy", true, "true", ""},
		{"- [ ] Howdy", true, "false", ""},
		{"not a checkbox", false, "not a checkbox", "could not parse checkbox"},
	}
	runValidationTests(t, testCases, ParseCheckbox, "ParseCheckbox")
}

func TestParseNumber(t *testing.T) {
	testCases := []struct {
		input         string
		expectedPass  bool
		expectedValue int
		expectedError string
	}{
		{"123", true, 123, ""},
		{"abc123", true, 123, ""},
		{"123abc", true, 123, ""},
		{"abc", false, 0, "could not be parsed into a number"},
	}

	for _, tc := range testCases {
		pass, value, _ := ParseNumber(tc.input)
		if pass != tc.expectedPass {
			t.Errorf("ParseNumber(%s) passing is %v, expected %v", tc.input, pass, tc.expectedPass)
		}

		if value != tc.expectedValue {
			t.Errorf("ParseNumber(%s) got new value %v, expected %v", tc.input, value, tc.expectedValue)
		}
	}
}

func TestParseBool(t *testing.T) {
	testCases := []struct {
		input         string
		expectedPass  bool
		expectedValue bool
		expectedError string
	}{
		{"true", true, true, ""},
		{"false", true, false, ""},
		{"not a bool", false, false, "could not be parsed into a boolean"},
	}

	for _, tc := range testCases {
		pass, value, _ := ParseBool(tc.input)
		if pass != tc.expectedPass {
			t.Errorf("ParseBool(%s) passing is %v, expected %v", tc.input, pass, tc.expectedPass)
		}

		if value != tc.expectedValue {
			t.Errorf("ParseBool(%s) got new value %v, expected %v", tc.input, value, tc.expectedValue)
		}
	}
}

func TestIsPresent(t *testing.T) {
	testCases := []validationTestCase{
		{"", false, "", "is empty"},
		{"foo", true, "foo", ""},
	}
	runValidationTests(t, testCases, IsPresent, "IsPresent")
}

func TestIsEmail(t *testing.T) {
	testCases := []validationTestCase{
		{"test@example.com", true, "test@example.com", ""},
		{"@example.com", false, "@example.com", ""},
		{"", false, "", ""},
	}
	runValidationTests(t, testCases, IsEmail, "IsEmail")
}

func TestIsUrl(t *testing.T) {
	testCases := []validationTestCase{
		{"https://www.com", true, "https://www.com", ""},
		{"ftp://www.com", false, "ftp://www.com", ""},
		{"example.com", false, "example.com", ""},
		{"foo", false, "foo", ""},
		{"", false, "", ""},
	}
	runValidationTests(t, testCases, IsURL, "IsUrl")
}

func TestIsChecked(t *testing.T) {
	testCases := []validationTestCase{
		{"", false, "", "must be checked"},
	}
	runValidationTests(t, testCases, IsChecked, "IsChecked")
}
