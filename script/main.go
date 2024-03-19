package main

import (
	"fmt"
	"log"
	"os"
)

func printUsageAndExit() {
	log.Fatalf("Usage: ./processor <review> [--test-issue <issue name>]")
}

func getEnv(key string) (string, error) {
	value := os.Getenv(key)

	if value == "" {
		return "", fmt.Errorf("missing variable %s", key)
	}

	return value, nil
}

func main() {
	if len(os.Args) < 2 {
		printUsageAndExit()
	}

	command := os.Args[1]

	if len(os.Args) == 4 && os.Args[2] == "--test-issue" {
		testIssueName = os.Args[3]
		debugMessage(fmt.Sprintf("Using test issue '%s'", testIssueName))
	} else if len(os.Args) != 2 {
		printUsageAndExit()
	}

	switch command {
	case "review":
		reviewer := Reviewer{}
		reviewer.Review()
	default:
		fmt.Printf("Invalid command: %s\n", command)
		printUsageAndExit()
	}
}
