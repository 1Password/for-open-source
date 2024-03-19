package main

import (
	"fmt"
	"os"
)

func printUsageAndExit() {
	fmt.Println("Usage: ./processor <command> [--test-issue <issue name>]")
	os.Exit(1)
}

func main() {
	if len(os.Args) < 2 {
		printUsageAndExit()
	}

	command := os.Args[1]

	if len(os.Args) == 4 && os.Args[2] == "--test-issue" {
		testIssueName = os.Args[3]
		testMessage(fmt.Sprintf("Using test issue '%s'", testIssueName))
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
