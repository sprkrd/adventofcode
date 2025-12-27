#!/usr/bin/env python3

import json
import os
import requests
import shutil
import sys
import time

from argparse import ArgumentParser
from html.parser import HTMLParser


ROOT_PATH = os.path.dirname(__file__)
DEFAULT_COOKIE_PATH = os.path.join(ROOT_PATH, "session-cookie.json")
TEMPLATE_PATH = os.path.join(ROOT_PATH, "template")
AOC_URL = "https://adventofcode.com"


class CodeSnippetsParser(HTMLParser):
    def __init__(self, all_samples=False):
        super().__init__()
        self._tag_stk = []
        self._code_snippets = []
        self._all_samples = all_samples

    def handle_starttag(self, tag, attrs):
        self._tag_stk.append(tag)

    def handle_endtag(self, tag):
        self._tag_stk.pop()

    def handle_data(self, data):
        store = (self._all_samples or not self._code_snippets)
        store = store and self._tag_stk[-2:] == ["pre", "code"]
        if store:
            self._code_snippets.append(data)

    def parse(self, data):
        self.feed(data)
        return self._code_snippets


def print_error_and_exit(error_msg):
    print(error_msg, file=sys.stderr)
    sys.exit(1)


def prepare_day(year, day, session_cookie, force_download=False, all_samples=False):
    print(f"Preparing {year}/{day}...")
    dst_path = os.path.join(ROOT_PATH, str(year), str(day))
    input_path = os.path.join(dst_path, "input")
    cookies = {"session": session_cookie}
    if os.path.isdir(dst_path):
        print("> Working directory already exists")
    else:
        print("> Setting up working directory")
        shutil.copytree(TEMPLATE_PATH, dst_path)

    day_url = f"{AOC_URL}/{year}/day/{day}"
    if os.path.isfile(input_path) and not force_download:
        print("> Input already downloaded. Nothing to do.")
    else:
        print("> Downloading input...", end="")
        try:
            input_url = f"{day_url}/input"
            response = requests.get(input_url, cookies=cookies)
            if not response.ok:
                raise RuntimeError(f"Status: {response.status_code}")
            with open(input_path, "wb") as f:
                f.write(response.content)
            print(" Done.")
        except Exception as e:
            print(f" Couldn't download input: {e}")

    try:
        print("> Parsing code blocks...", end="")
        response = requests.get(day_url, cookies=cookies)
        if not response.ok:
            raise RuntimeError(f"Status: {response.status_code}")
        code_snippets_parser = CodeSnippetsParser(all_samples=all_samples)
        code_snippets = code_snippets_parser.parse(response.text)
        for i, code_snippet in enumerate(code_snippets, 1):
            snippet_filename = os.path.join(dst_path, f"sample{i}")
            print(f" {os.path.basename(snippet_filename)}", end="")
            with open(snippet_filename, "w") as f:
                f.write(code_snippet)
        print(". Done.")
    except Exception as e:
        print(f" Error parsing code snippets: {e}")
        raise e


def main():
    argument_parser = ArgumentParser(
        prog=sys.argv[0],
        description="Automatically prepares working directory and downloads AOC puzzle inputs for one or a range of days")
    argument_parser.add_argument("year", type=int)
    argument_parser.add_argument("day", type=int)
    argument_parser.add_argument(
            "-f", "--force-download", action="store_true",
            help="Download the input even if the input file already exists")
    argument_parser.add_argument(
            "-a", "--all-samples", action="store_true",
            help="Save all the code samples, not just the first one")
    argument_parser.add_argument("-l", "--last-day",
                                 type=int,
                                 help="If specified, all the days between day and last-day will be prepared")
    argument_parser.add_argument(
            "-c", "--cookie-file",
            help="If not specified, uses the session-cookie.json file alongside this script")

    args = argument_parser.parse_args()

    first_day = args.day
    last_day = first_day
    if args.last_day:
        last_day = args.last_day        

    if args.cookie_file:
        cookie_file = args.cookie_file
    else:
        cookie_file = DEFAULT_COOKIE_PATH

    try:
        with open(cookie_file, "r") as f:
            cookies = json.load(f)
            session_cookie = cookies["session"]
    except IOError:
        print_error_and_exit(f"Cannot open {cookie_file}")
    except Exception as e:
        print_error_and_exit(f"Cannot find session key in {cookie_file}: {e}")

    for day in range(first_day, last_day+1):
        if day != first_day:
            sleep(0.5) # courtesy time between requests
        prepare_day(args.year, day, session_cookie, args.force_download, args.all_samples)


if __name__ == "__main__":
    main()


