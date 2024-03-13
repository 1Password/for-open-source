package main

import (
	"fmt"
	"os"
)

func printUsageAndExit() {
	fmt.Println("Usage: ./processor <validate>")
	os.Exit(1)
}

func main() {
	if len(os.Args) != 2 {
		printUsageAndExit()
	}

	command := os.Args[1]
	switch command {
	case "validate":
		// handle app validation
	default:
		fmt.Printf("Invalid command: %s\n", command)
		printUsageAndExit()
	}
}
