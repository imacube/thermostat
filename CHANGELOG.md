# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.2.0] - 2020-02-06
### Added
- Test for cool relay staying on.
- Test results are stored in a set, any return values of `1` are considered a test failure.
- `Makefile` targets for uploading to Arduinos.

### Changed
- Messages to be printed during a test failure are now stored in a variable for
  latter printing if there is an error.
- `Makefile` targets reorganized for better clarity.

## [1.1.0] - 2020-02-01
### Added
- The `Makefile` can now manage all Arduino needs except uploading the sketch.

## [1.0.0] - 2020-01-28
### Added
- Initial version.
- `README.md` has details on the repo.
