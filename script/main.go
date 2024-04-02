package main

import (
	"fmt"
	"log"
	"os"
)

func printUsageAndExit() {
	log.Fatal("Usage: ./processor <review|approve> [--test <name>]")
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

	if len(os.Args) == 4 && os.Args[2] == "--test" {
		setTestApplication(os.Args[3])
	} else if len(os.Args) != 2 {
		printUsageAndExit()
	}

	switch command {
	case "review":
		reviewer := Reviewer{}
		reviewer.Review()
	case "approve":
		approver := Approver{}
		approver.Approve()
	default:
		fmt.Printf("Invalid command: %s\n", command)
		printUsageAndExit()
	}
}
