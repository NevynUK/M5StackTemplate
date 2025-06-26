import os as os
from shutil import copytree
import re
import sys
from re import sub
from datetime import datetime

script_directory = os.path.dirname(os.path.abspath(sys.argv[0])) + "/"


def toSnakeCase(sentence):
    # Replace hyphens with spaces, then apply regex replacement for title case conversion,
    # add underscores between words, and finally convert the result to lowercase
    return '_'.join(
        sub('([A-Z][a-z]+)', r' \1',
            sub('([A-Z]+)', r' \1',
                sentence.replace('-', ' '))).split()).lower()


def toCamelCase(sentence):
    # Use regex to replace underscores and hyphens with spaces,
    # then convert the string to title format (capitalize first letter of each word) and remove spaces
    sentence = sub(r"(_|-)+", " ", sentence).title().replace(" ", "")

    # Ensure the first letter is lowercase
    return ''.join([sentence[0].lower(), sentence[1:]])


def toCamelCase(sentence):
    words = sentence.replace('_', ' ').split()
    camel_case = ''.join(word.capitalize() for word in words)
    return camel_case


def checkInputValid(input):
    # Check if characters are valid
    if input == "":
        return False
    if not re.match("^[a-zA-Z_\s]+$", input):
        return False
    if input == "template":
        return False

    # Check if format is valid
    if len(input.replace(' ', '').replace('_', '')) == 0:
        return False

    return True


class PrintColor:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'


def getAppName():
    # Read input
    while True:
        user_input = input("input new app's name: ")
        if checkInputValid(user_input):
            break
        else:
            print("bad name, try again")

    print("get input: {}\n".format(user_input))

    app_name = "App" + toCamelCase(user_input)
    print("> new app's name: " + PrintColor.OKGREEN + app_name + PrintColor.ENDC)
    return app_name


class AppFilePaths:
    def __init__(self, folder, source_file, header_file):
        self.folder = folder
        self.source_file = source_file
        self.header_file = header_file


def getAppFilePaths(appName):
    file_paths = AppFilePaths("", "", "")

    # Get file paths
    file_paths.folder = script_directory + toSnakeCase(appName)

    file_paths.source_file = os.path.join(
        file_paths.folder, toSnakeCase(appName) + ".cpp")

    file_paths.header_file = os.path.join(
        file_paths.folder, toSnakeCase(appName) + ".h")

    print("> new app's file paths:\n - {}\n - {}\n - {}\n".format(
        PrintColor.OKGREEN + file_paths.folder,
        PrintColor.OKGREEN + file_paths.source_file,
        PrintColor.OKGREEN + file_paths.header_file)
        + PrintColor.ENDC)
    return file_paths


def createAppFiles(appName, appFilePaths):
    # Create folder
    os.mkdir(appFilePaths.folder)

    # Create files
    source_file = open(appFilePaths.source_file, mode='x')
    header_file = open(appFilePaths.header_file, mode='x')

    # Get template content
    content_source_file = open(
        script_directory + "app_template/app_template.cpp", mode='r').read()
    content_header_file = open(
        script_directory + "app_template/app_template.h", mode='r').read()

    # Replace class name
    content_source_file = content_source_file.replace(
        "AppTemplate", appName)
    content_header_file = content_header_file.replace(
        "AppTemplate", appName)

    # Replace file name
    content_source_file = content_source_file.replace(
        "app_template", toSnakeCase(appName))
    content_header_file = content_header_file.replace(
        "app_template", toSnakeCase(appName))

    # Replace date
    content_source_file = content_source_file.replace(
        "<date></date>", datetime.now().strftime("%Y-%m-%d"))
    content_header_file = content_header_file.replace(
        "<date></date>", datetime.now().strftime("%Y-%m-%d"))

    # Write content
    source_file.write(content_source_file)
    header_file.write(content_header_file)

    # Close files
    source_file.close()
    header_file.close()


def installApp(appName):
    print("> install app {} into mooncake".format(appName))

    # Read app installation callback
    app_install_cb_file = open(script_directory + "app_installer.h", mode='r')
    content_app_install_cb_file = app_install_cb_file.read()
    app_install_cb_file.close()

    # Add header file include
    header_include_tag = "/* Header files locator (Don't remove) */"
    content_app_install_cb_file = content_app_install_cb_file.replace(
        header_include_tag, "#include \"{}/{}.h\"\n{}".format(toSnakeCase(appName), toSnakeCase(appName), header_include_tag))

    # Add app installation statement
    app_install_tag = "/* Install app locator (Don't remove) */"
    content_app_install_cb_file = content_app_install_cb_file.replace(
        app_install_tag, "mooncake::GetMooncake().installApp(std::make_unique<{}>());\n    {}".format(appName, app_install_tag))

    # Write app installation callback
    app_install_cb_file = open(script_directory + "app_installer.h", mode='w+')
    app_install_cb_file.write(content_app_install_cb_file)
    app_install_cb_file.close()


if __name__ == "__main__": 
    print("app generator")

    name = getAppName()  # Get app name
    file_paths = getAppFilePaths(name)  # Get file paths
    createAppFiles(name, file_paths)  # Create app files
    installApp(name)  # Install app

    print("\ndone")  # Complete
