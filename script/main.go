package main

import (
	"fmt"
	"os"
)

func printUsageAndExit() {
	fmt.Println("Usage: ./processor <review>")
	os.Exit(1)
}

func main() {
	if len(os.Args) != 2 {
		printUsageAndExit()
	}

	command := os.Args[1]
	switch command {
	case "review":
		// handle app review
	default:
		fmt.Printf("Invalid command: %s\n", command)
		printUsageAndExit()
	}
}
