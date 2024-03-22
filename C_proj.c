#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>     // This one to do math calculation
#include <limits.h>   // Add this line to include limits.h
#include <ctype.h>    // This is used for isdigit()
#include <sys/stat.h> // Include for checking file existence

#define MAX_CATEGORIES 12
#define MAX_NAME_LEN 15
#define MAX_TITLE_LEN 50
#define CHART_WIDTH 150
#define CHART_HEIGHT 35

typedef struct
{
    char name[MAX_NAME_LEN + 1];
    int value;
} Category;

Category categories[MAX_CATEGORIES];
int num_categories = 0;
char chart_title[MAX_TITLE_LEN + 1];
char x_axis_label[MAX_TITLE_LEN + 1];
char sort_preference;

// Function Prototypes
void getUserInput();
void sortCategories();
void displayChart();
void displayVerticalChart();
void saveChartToFile();
void displayChartToFile(FILE *file);
int compareByName(const void *a, const void *b);
int compareByValue(const void *a, const void *b);
int getMaxValue();
int isString(const char *str);
int fileExists(const char *filename);
int loadChartFromFile(const char *filename);
void editChartData();
int promptLoadChart();

int main()
{
    printf("Horizontal Bar Chart Generator\n");

    if (promptLoadChart())
    {
        char filename[256];
        printf("Enter the filename to load: ");
        if (fgets(filename, sizeof(filename), stdin))
        {
            filename[strcspn(filename, "\n")] = '\0'; // Remove newline character
            // Append ".txt" extension to the file name
            strcat(filename, ".txt");
            if (!loadChartFromFile(filename))
            {
                printf("Failed to load the chart. Creating a new chart instead.\n");
                getUserInput();
            }
            else
            {
                editChartData();
            }
        }
    }
    else
    {
        getUserInput();
    }

    sortCategories();
    displayChart();
    char verticalChoice;
    printf("Would you like to view the graph vertically (y/n)? ");
    scanf(" %c", &verticalChoice); // Correct usage of scanf to consume the newline.
    
    if (verticalChoice == 'y' || verticalChoice == 'Y') {
        displayVerticalChart(); // Function to display the vertical chart.
    }
    saveChartToFile();

    return 0;
}


// All of my different functions

void getUserInput()
{
    while (1)
    {
        printf("Enter chart title: ");
        fgets(chart_title, MAX_TITLE_LEN, stdin);
        chart_title[strcspn(chart_title, "\n")] = 0; // Remove newline character

        if (!isString(chart_title))
        {
            printf("Invalid input. Please enter a non-numeric title.\n");
        }
        else
        {
            break; // Valid input
        }
    }

    while (1)
    {
        printf("Enter X-axis label: ");
        fgets(x_axis_label, MAX_TITLE_LEN, stdin);
        x_axis_label[strcspn(x_axis_label, "\n")] = 0; // Remove newline character

        if (!isString(x_axis_label))
        {
            printf("Invalid input. Please enter a non-numeric label.\n");
        }
        else
        {
            break; // Valid input
        }
    }

    printf("How many categories (1-%d)? ", MAX_CATEGORIES);
    while (1)
    {
        char input[256];
        fgets(input, 256, stdin); // Use fgets to read a string from stdin

        // Attempt to convert the string to an integer
        char *endPtr;
        long int value = strtol(input, &endPtr, 10);

        // Check if the input was an integer and within the valid range
        if (endPtr != input && *endPtr == '\n' && value >= 1 && value <= MAX_CATEGORIES)
        {
            num_categories = (int)value;
            break; // Valid input, exit the loop
        }
        else
        {
            printf("Invalid input. Please enter a number between 1 and %d: ", MAX_CATEGORIES);
        }
    }

    for (int i = 0; i < num_categories; i++)
    {

        while (1)
        { // Loop until valid input is provided
            printf("Enter category %d name: ", i + 1);
            fgets(categories[i].name, MAX_NAME_LEN, stdin);
            categories[i].name[strcspn(categories[i].name, "\n")] = 0; // Remove newline character

            if (!isString(categories[i].name))
            {
                printf("Invalid input. Please enter a non-numeric name.\n");
            }
            else
            {
                break; // Valid input, exit the loop
            }
        }

        while (1)
        {
            printf("Enter category %d value (integer): ", i + 1);
            char input[256];
            fgets(input, 256, stdin); // Use fgets to read a string from stdin

            char *endPtr;
            long int value = strtol(input, &endPtr, 10);

            if (endPtr != input && *endPtr == '\n' && value >= INT_MIN && value <= INT_MAX)
            {
                categories[i].value = (int)value;
                break; // Valid input, exit the loop
            }
            else
            {
                printf("Invalid input. Please enter an integer value: ");
            }
        }
    }

    char sortInput[3]; // Buffer to store the input, including space for newline and null terminator

    while (1)
    {
        printf("Sort by name (N) or value (V)?: ");
        if (fgets(sortInput, sizeof(sortInput), stdin))
        {
            // Check if the first character is valid and the second character is a newline
            if ((sortInput[0] == 'n' || sortInput[0] == 'N' || sortInput[0] == 'v' || sortInput[0] == 'V') &&
                sortInput[1] == '\n')
            {
                sort_preference = tolower(sortInput[0]); // Convert to lowercase and set the sorting preference
                break;                                   // Valid input, exit the loop
            }
            else
            {
                // Not valid input or buffer not empty, clear the buffer
                while (fgets(sortInput, sizeof(sortInput), stdin))
                {
                    if (strchr(sortInput, '\n'))
                        break; // Newline found, buffer cleared
                }
                printf("Invalid input. Please enter 'N'/'n' or 'V'/'v'.\n");
            }
        }
        else
        {
            // fgets failed, handle error
            printf("Error reading input. Please try again.\n");
            clearerr(stdin); // Clear error state
            while (fgets(sortInput, sizeof(sortInput), stdin))
            {
                if (strchr(sortInput, '\n'))
                    break; // Newline found, buffer cleared
            }
        }
    }
}

void sortCategories()
{
    if (sort_preference == 'n')
    {
        qsort(categories, num_categories, sizeof(Category), compareByName);
    }
    else
    {
        qsort(categories, num_categories, sizeof(Category), compareByValue);
    }
}

int compareByName(const void *a, const void *b)
{
    return strcmp(((Category *)a)->name, ((Category *)b)->name);
}

int compareByValue(const void *a, const void *b)
{
    return ((Category *)b)->value - ((Category *)a)->value; // For descending order
}

void displayChart()
{
    int maxValue = getMaxValue();
    int longestBar = CHART_WIDTH - MAX_NAME_LEN - 3; // Adjust the longest bar length for padding
    double scale = (double)maxValue / longestBar;
    int scaleMagnitude = scale > 1 ? (int)(ceil(log10(scale) / 3) * 3) : 0;
    double scaledMaxValue = maxValue / pow(10, scaleMagnitude);

    // Center the chart title relative to the longest bar
    int titleLength = strlen(chart_title);
    int graphStartPadding = MAX_NAME_LEN + 4;         // Space taken by category names and padding
    int graphWidth = CHART_WIDTH - graphStartPadding; // Total width available for graph bars
    int titlePadding = (graphWidth - titleLength) / 2 + graphStartPadding;

    printf("%*s%s\n\n", titlePadding, "", chart_title);

    // Display each category and their scaled bar length
    for (int i = 0; i < num_categories; i++)
    {
        printf("%-*s | ", MAX_NAME_LEN, categories[i].name);
        int barLength = (int)(categories[i].value / scale);
        for (int j = 0; j < barLength; j++)
        {
            printf("X");
        }
        printf("\n");
    }

    // Draw the x-axis and place '+' for tick marks
    printf("%-*s+", MAX_NAME_LEN + 2, ""); // Start of the x-axis
    for (int i = 0; i < longestBar; i++)
    {
        if (i == longestBar / 2)
        {
            printf("+"); // Middle tick mark
        }
        else
        {
            printf("-");
        }
    }
    printf("+\n"); // End of the x-axis

    // Place tick mark labels at the start, middle, and end
    int labelWidth = (int)log10((int)scaledMaxValue) + 1;                            // Calculate the label width of the largest number
    printf("%-*s0", MAX_NAME_LEN + 2, "");                                           // Start label
    printf("%*s%d", longestBar / 2 - labelWidth / 2, "", (int)(scaledMaxValue / 2)); // Middle label
    printf("%*d\n", longestBar / 2 - labelWidth / 2, (int)scaledMaxValue);           // End label

    char scaleLabel[50];
    if (scaleMagnitude > 0)
    {
        sprintf(scaleLabel, "%s (x%d)", x_axis_label, (int)pow(10, scaleMagnitude));
    }
    else
    {
        strcpy(scaleLabel, x_axis_label);
    }

    int scaleLabelLength = strlen(scaleLabel);
    int scaleLabelPadding = (graphWidth - scaleLabelLength) / 2 + graphStartPadding;
    printf("%*s%s\n", scaleLabelPadding, "", scaleLabel); // Centered x-axis label
}

int getMaxValue()
{
    int max = 0;
    for (int i = 0; i < num_categories; i++)
    {
        if (categories[i].value > max)
        {
            max = categories[i].value;
        }
    }
    return max;
}

void displayVerticalChart() {
    int maxBarHeight = 10; // Maximum 10 rows for vertical bars
    int maxValue = getMaxValue();
    double scale = (double)maxValue / (double)maxBarHeight;
    
    // Center the title and x-axis label
    printf("%*s\n", (CHART_WIDTH + strlen(chart_title)) / 2, chart_title);
    printf("%*s\n\n", (CHART_WIDTH + strlen(x_axis_label)) / 2, x_axis_label);

    // Calculate the space each category takes including the bar, name, and space between them
    int categorySpace = MAX_NAME_LEN + 3; // Space for bar and between bars

    // Center the chart by calculating left padding
    int chartPadding = (CHART_WIDTH - (categorySpace * num_categories)) / 2;
    chartPadding = chartPadding < 0 ? 0 : chartPadding; // Ensure padding isn't negative

    // Print the vertical bars
    for (int row = 0; row < maxBarHeight; row++) {
        // Print left padding for centering the chart
        printf("%*s", chartPadding, "");

        // Print bars
        for (int i = 0; i < num_categories; i++) {
            int barHeight = (int)(categories[i].value / scale);
            printf("%c%-*s", barHeight >= maxBarHeight - row ? '#' : ' ', MAX_NAME_LEN, "");
        }

        // Print the Y-axis on the right side of the graph
        if (row == 0) {
            printf("| %d", maxValue); // Top tick with max value label
        } else if (row == maxBarHeight / 2) {
            printf("| %d", maxValue / 2); // Middle tick with mid value label
        } else if (row == maxBarHeight - 1) {
            printf("| 0"); // Bottom tick with 0 label
        } else {
            printf("|"); // Y-axis line
        }
        printf("\n");
    }

    // Print category names and values below the chart
    printf("%*s", chartPadding, ""); // Left padding for centering the chart
    for (int i = 0; i < num_categories; i++) {
        printf("%-*s", MAX_NAME_LEN, categories[i].name);
    }
    printf("\n");

    printf("%*s", chartPadding, ""); // Left padding for centering the chart
    for (int i = 0; i < num_categories; i++) {
        printf("%-*d", MAX_NAME_LEN, categories[i].value);
    }
    printf("\n");
}

void saveChartToFile()
{
    // Clear the input buffer before reading new input
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    char choice[3]; // Buffer for choice to include newline and null terminator
    printf("Do you want to save the chart to a file? (Y/N): ");

    while (fgets(choice, sizeof(choice), stdin))
    {
        // Handling buffer clearing if input is longer than expected
        if (choice[strlen(choice) - 1] != '\n') {
            while ((c = getchar()) != '\n' && c != EOF) { }
        }

        // Check if only 'y', 'Y', 'n', or 'N' was entered
        if (strchr("yYnN", choice[0]) && (choice[1] == '\n' || choice[1] == '\0'))
        {
            if (choice[0] == 'n' || choice[0] == 'N')
            {
                printf("Chart will not be saved.\n");
                return; // Exit if user does not want to save the chart
            }
            break; // Valid input
        }
        else
        {
            printf("Invalid input. Please enter 'Y'/'y' to save or 'N'/'n' to cancel: ");
        }
    }

    if (tolower(choice[0]) == 'y')
    {
        char fileName[256]; // Buffer to hold the file name

        printf("Enter the name of the file to save the chart (will append '.txt'): ");
        fgets(fileName, sizeof(fileName), stdin);
        fileName[strcspn(fileName, "\n")] = 0; // Remove newline character

        // Append ".txt" extension to the file name
        strcat(fileName, ".txt");

        // Handling file existence check and potential overwriting
        if (fileExists(fileName))
        {
            printf("File '%s' already exists. Overwrite? (Y/N): ", fileName);
            fgets(choice, sizeof(choice), stdin);

            // Clearing buffer if input is longer than expected
            if (choice[strlen(choice) - 1] != '\n') {
                while ((c = getchar()) != '\n' && c != EOF) { }
            }

            if (tolower(choice[0]) != 'y')
            {
                printf("Chart not saved.\n");
                return; // Do not overwrite file, exit function
            }
        }

        FILE *file = fopen(fileName, "w");
        if (!file)
        {
            printf("Error opening file for writing.\n");
            return;
        }

        displayChartToFile(file); // Write the chart to the file
        printf("Chart saved to %s\n", fileName);
        fclose(file);
    }
}

void displayChartToFile(FILE *file)
{
    int maxValue = getMaxValue();
    int longestBar = CHART_WIDTH - MAX_NAME_LEN - 3;
    double scale = (double)maxValue / longestBar;
    int scaleMagnitude = scale > 1 ? (int)(ceil(log10(scale) / 3) * 3) : 0;
    double scaledMaxValue = maxValue / pow(10, scaleMagnitude);

    // Center the chart title relative to the longest bar
    int titleLength = strlen(chart_title);
    int graphStartPadding = MAX_NAME_LEN + 4;         // Space taken by category names and padding
    int graphWidth = CHART_WIDTH - graphStartPadding; // Total width available for graph bars
    int titlePadding = (graphWidth - titleLength) / 2 + graphStartPadding;
    fprintf(file, "%*s%s\n\n", titlePadding, "", chart_title);

    // Write each category and their scaled bar length
    for (int i = 0; i < num_categories; i++)
    {
        fprintf(file, "%-*s | ", MAX_NAME_LEN, categories[i].name);
        int barLength = (int)(categories[i].value / scale);
        for (int j = 0; j < barLength; j++)
        {
            fprintf(file, "X");
        }
        fprintf(file, "\n");
    }

    // Write the x-axis and '+' for tick marks
    fprintf(file, "%-*s+", MAX_NAME_LEN + 2, "");
    for (int i = 0; i < longestBar; i++)
    {
        if (i == longestBar / 2)
        {
            fprintf(file, "+");
        }
        else
        {
            fprintf(file, "-");
        }
    }
    fprintf(file, "+\n");

    // Write tick mark labels at the start, middle, and end
    int labelWidth = (int)log10((int)scaledMaxValue) + 1;                                   // Calculate the label width of the largest number
    fprintf(file, "%-*s0", MAX_NAME_LEN + 2, "");                                           // Start label
    fprintf(file, "%*s%d", longestBar / 2 - labelWidth / 2, "", (int)(scaledMaxValue / 2)); // Middle label
    fprintf(file, "%*d\n", longestBar / 2 - labelWidth / 2, (int)scaledMaxValue);           // End label

    // Center the x-axis label with scaling factor
    char scaleLabel[50];
    if (scaleMagnitude > 0)
    {
        sprintf(scaleLabel, "%s (x%d)", x_axis_label, (int)pow(10, scaleMagnitude));
    }
    else
    {
        strcpy(scaleLabel, x_axis_label);
    }

    int scaleLabelPadding = (graphWidth - strlen(scaleLabel)) / 2 + graphStartPadding;
    fprintf(file, "%*s%s\n", scaleLabelPadding, "", scaleLabel);

    // Write the structured data at the end of the file with better formatting
    fprintf(file, "\n---\n");                                    // Delimiter to separate the visual chart from the data
    fprintf(file, "Title: %s\n", chart_title);                   // Write the chart title
    fprintf(file, "X-axis Label: %s\n", x_axis_label);           // Write the x-axis label
    fprintf(file, "Number of Categories: %d\n", num_categories); // Write the number of categories
    fprintf(file, "\nCategories:\n");                            // A header for the categories section

    // Write each category's name and value with indentation for better readability
    for (int i = 0; i < num_categories; i++)
    {
        fprintf(file, "  %s : %d\n", categories[i].name, categories[i].value);
    }
}

int isString(const char *str)
{
    while (*str)
    {
        if (isdigit((unsigned char)*str))
        {
            return 0; // Found a digit, return false
        }
        str++;
    }
    return 1; // No digits found, return true
}

int fileExists(const char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int loadChartFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("File not found or could not be opened.\n");
        return 0; // Indicate failure
    }

    char buffer[1024];
    // Read lines until you find the delimiter '---'
    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strcmp(buffer, "---\n") == 0)
        {
            break; // Delimiter found, stop reading further
        }
    }

    // Read the chart title, prefixed with "Title: "
    if (fscanf(file, "Title: %50[^\n]\n", chart_title) != 1)
    {
        printf("Failed to read chart title from file.\n");
        fclose(file);
        return 0;
    }

    // Read the x-axis label, prefixed with "X-axis Label: "
    if (fscanf(file, "X-axis Label: %50[^\n]\n", x_axis_label) != 1)
    {
        printf("Failed to read x-axis label from file.\n");
        fclose(file);
        return 0;
    }

    // Read the number of categories, prefixed with "Number of Categories: "
    if (fscanf(file, "Number of Categories: %d\n", &num_categories) != 1)
    {
        printf("Failed to read the number of categories from file.\n");
        fclose(file);
        return 0;
    }

    if (num_categories < 1 || num_categories > MAX_CATEGORIES)
    {
        printf("Number of categories is outside the valid range.\n");
        fclose(file);
        return 0;
    }

    // Skip the line that says "Categories:"
    fgets(buffer, sizeof(buffer), file);

    // Read categories, now prefixed with the category name
    for (int i = 0; i < num_categories; i++)
    {
        if (fscanf(file, "%15[^:]: %d\n", categories[i].name, &categories[i].value) != 2)
        {
            printf("Failed to read data for category %d.\n", i + 1);
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1; // Success
}



void editChartData()
{
    int choice;
    char input[256]; // Buffer to store user input as string

    do
    {
        // Display menu
        printf("\nEdit Chart Data Menu:\n");
        printf("1. Edit chart title\n");
        printf("2. Edit x-axis label\n");
        printf("3. Edit category names and values\n");
        printf("4. Change the number of categories\n");
        printf("5. Exit\n");
        printf("Enter your choice (1-5): ");

        fgets(input, sizeof(input), stdin); // Read input as string
        // Try to parse integer, re-prompt if not successful
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("Invalid choice, please enter a number between 1 and 5.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Current chart title: %s\n", chart_title);
            printf("Enter new chart title: ");
            fgets(chart_title, MAX_TITLE_LEN, stdin);
            chart_title[strcspn(chart_title, "\n")] = '\0'; // Remove newline character
            printf("Chart title updated to: %s\n", chart_title);
            break;

        case 2:
            printf("Current x-axis label: %s\n", x_axis_label);
            printf("Enter new x-axis label: ");
            fgets(x_axis_label, MAX_TITLE_LEN, stdin);
            x_axis_label[strcspn(x_axis_label, "\n")] = '\0'; // Remove newline character
            printf("X-axis label updated to: %s\n", x_axis_label);
            break;

        case 3:
            for (int i = 0; i < num_categories; i++)
            {
                printf("Current name for category %d: %s\n", i + 1, categories[i].name);
                printf("Enter new name for category %d: ", i + 1);
                fgets(categories[i].name, MAX_NAME_LEN, stdin);
                categories[i].name[strcspn(categories[i].name, "\n")] = '\0'; // Remove newline character

                int value;
                int readValueSuccess = 0;
                while (!readValueSuccess)
                {
                    printf("Current value for category %d: %d\n", i + 1, categories[i].value);
                    printf("Enter new value for category %d: ", i + 1);
                    fgets(input, sizeof(input), stdin); // Read input as string

                    // Check if the input is a valid integer
                    if (sscanf(input, "%d", &value) == 1)
                    {
                        categories[i].value = value;
                        readValueSuccess = 1; // Break the loop if input is valid
                    }
                    else
                    {
                        printf("Invalid input. Please enter an integer value.\n");
                    }
                }
            }
            break;

        case 4:
            printf("Current number of categories: %d\n", num_categories);
            printf("Enter new number of categories (1-%d): ", MAX_CATEGORIES);
            int newNumCategories;
            fgets(input, sizeof(input), stdin); // Read input as string
            // Check if the input is a valid integer within range
            if (sscanf(input, "%d", &newNumCategories) == 1 && newNumCategories > 0 && newNumCategories <= MAX_CATEGORIES)
            {
                // If valid, update the number of categories
                num_categories = newNumCategories;
                // Prompt for new categories data
                for (int i = 0; i < num_categories; i++)
                {
                    printf("Enter name for category %d: ", i + 1);
                    fgets(categories[i].name, MAX_NAME_LEN, stdin);
                    categories[i].name[strcspn(categories[i].name, "\n")] = '\0'; // Remove newline character

                    printf("Enter value for category %d: ", i + 1);
                    fgets(input, sizeof(input), stdin); // Read input as string
                    // Check if the input is a valid integer
                    while (sscanf(input, "%d", &categories[i].value) != 1)
                    {
                        printf("Invalid input. Please enter an integer value for category %d: ", i + 1);
                        fgets(input, sizeof(input), stdin); // Read input as string
                    }
                }
                printf("Number of categories and their values updated.\n");
            }
            else
            {
                printf("Invalid input. Please enter a number between 1 and %d.\n", MAX_CATEGORIES);
            }
            break;

        case 5:
            printf("Exiting edit menu.\n");
            break;

        default:
            printf("Invalid choice, please enter a number between 1 and 5.\n");
        }
    } while (choice != 5);
}

int promptLoadChart()
{
    char editChoice[256]; // Use a larger buffer for safer input handling
    printf("Load existing chart for editing? (Y/N): ");
    if (fgets(editChoice, sizeof(editChoice), stdin))
    {
        // Check if the input is 'y', 'Y', 'n', or 'N', and if there's no extra characters
        if ((tolower(editChoice[0]) == 'y' || tolower(editChoice[0]) == 'n') &&
            (editChoice[1] == '\n' || (editChoice[1] == '\r' && editChoice[2] == '\n')))
        {
            return tolower(editChoice[0]) == 'y';
        }
        else
        {
            printf("Invalid input. Please enter 'Y' for yes or 'N' for no.\n");
            return 0; // Default to no
        }
    }
    // If fgets fails, default to no
    return 0;
}