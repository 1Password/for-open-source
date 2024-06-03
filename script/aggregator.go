package main

import (
	"encoding/json"
	"log"
	"os"
	"path/filepath"
	"time"

	"github.com/getsentry/sentry-go"
)

type ProjectEntry struct {
	Project   Project   `json:"project"`
	CreatedAt time.Time `json:"created_at"`
}

type Aggregator struct {
	gitHub *GitHub
}

func (a *Aggregator) Aggregate() {
	if err := a.gitHub.Init(); err != nil {
		a.logErrorAndExit("could not initialize GitHub client", err)
	}

	dataDir := "./data"
	var projectsList []ProjectEntry

	if err := filepath.Walk(dataDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		// There may in the future be a non-JSON file living in
		// this directory, so let's safeguard against that
		if !info.IsDir() && filepath.Ext(path) == ".json" {
			file, err := os.ReadFile(path)
			if err != nil {
				return err
			}

			var app Application
			err = json.Unmarshal(file, &app)
			if err != nil {
				return err
			}

			projectEntry := ProjectEntry{
				Project:   app.Project,
				CreatedAt: app.CreatedAt,
			}

			projectsList = append(projectsList, projectEntry)
		}

		return nil
	}); err != nil {
		a.logErrorAndExit("failed to aggregate applicant data", err)
	}

	aggregateData, err := json.Marshal(projectsList)
	if err != nil {
		a.logErrorAndExit("failed to marshal projects into JSON", err)
	}

	outputFile := "./data.json"

	if err := a.gitHub.UpdateFile(
		outputFile,
		aggregateData,
		"Update applicant data",
	); err != nil {
		a.logErrorAndExit(
			"could not create commit",
			err,
		)
	}
}

func (a *Aggregator) logErrorAndExit(message string, err error) {
	sentry.CaptureException(err)
	log.Fatalf("Error aggregating data: %s: %s\n", message, err.Error())
}
