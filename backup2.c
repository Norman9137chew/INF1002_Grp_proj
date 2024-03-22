#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CATEGORIES 12
#define MAX_NAME_LEN 15
#define MAX_TITLE_LEN 50
#define CHART_WIDTH 150
#define CHART_HEIGHT 15

typedef struct {
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

int main() {
    printf("Horizontal Bar Chart Generator\n");
    getUserInput();
    sortCategories();

    displayChart();

    char verticalChoice;
    printf("Would you like to view the graph vertically (y/n)? ");
    scanf(" %c", &verticalChoice); // Correct usage of scanf to consume the newline.
    
    if (verticalChoice == 'y' || verticalChoice == 'Y') {
        displayVerticalChart(); // Function to display the vertical chart.
    }
    
    char saveChoice;
    printf("Would you like to save the chart (y/n)? ");
    scanf(" %c", &saveChoice);
    
    if (saveChoice == 'y' || saveChoice == 'Y') {
        saveChartToFile();
    }

    return 0;
}

// Implement the getMaxValue function
int getMaxValue() {
    int max = 0;
    for (int i = 0; i < num_categories; i++) {
        if (categories[i].value > max) {
            max = categories[i].value;
        }
    }
    return max;
}

// All of my different functions

void getUserInput()
{
    printf("Enter chart title: ");
    fgets(chart_title, MAX_TITLE_LEN, stdin);
    chart_title[strcspn(chart_title, "\n")] = 0; // Remove newline character

    printf("Enter X-axis label: ");
    fgets(x_axis_label, MAX_TITLE_LEN, stdin);
    x_axis_label[strcspn(x_axis_label, "\n")] = 0; // Remove newline character

    printf("How many categories (1-%d)? ", MAX_CATEGORIES);
    scanf("%d", &num_categories);
    while (getchar() != '\n')
        ; // Clear the input buffer

    for (int i = 0; i < num_categories; i++)
    {
        printf("Enter category %d name: ", i + 1);
        fgets(categories[i].name, MAX_NAME_LEN, stdin);
        categories[i].name[strcspn(categories[i].name, "\n")] = 0; // Remove newline character

        printf("Enter category %d value: ", i + 1);
        scanf("%d", &categories[i].value);
        while (getchar() != '\n')
            ; // Clear the input buffer
    }

    printf("Sort by name (n) or value (v)? ");
    sort_preference = getchar();
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
    char choice;
    printf("Do you want to save the chart to a file? (y/n) ");

    // Use scanf to read a single character and skip the newline
    scanf(" %c", &choice);
    getchar(); // Consume any additional newline character in the buffer

    if (choice == 'y' || choice == 'Y')
    {
        char fileName[256]; // Buffer to hold the file name
        printf("Enter the name of the file to save the chart (including .txt extension): ");
        fgets(fileName, 256, stdin);
        fileName[strcspn(fileName, "\n")] = 0; // Remove newline character

        FILE *file = fopen(fileName, "w");
        if (file == NULL)
        {
            printf("Error opening file.\n");
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
}