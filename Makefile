PHONY: install_deps build_processor bump_version

install_deps:
	$(info Installing dependencies...)
	@cd ./script && go get -v -t -d .

build_processor:
	$(info Building processor...)
	@cd ./script && go build -v -o ../processor .

test:
	$(info Running tests...)
	@cd ./script && go test

bump_processor_version:
	$(info Bumping version...)
	@$(eval LAST_TAG=$(shell git rev-list --tags='processor-*' --max-count=1 | xargs -r git describe --tags --match 'processor-*'))
	@$(eval LAST_NUM=$(if $(LAST_TAG),$(shell echo $(LAST_TAG) | sed 's/processor-//'),0))
	@$(eval NEW_NUM=$(shell echo $$(($(LAST_NUM) + 1))))
	$(info Creating new processor tag: processor-$(NEW_NUM))
	@git tag processor-$(NEW_NUM)
	@git push --tags
