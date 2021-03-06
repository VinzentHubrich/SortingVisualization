#include <SFML/Graphics.hpp>

#include <iostream>
#include <list>

const unsigned int SCREENWIDTH = 1200;
const unsigned int SCREENHEIGHT = 700;

const float barHeightScale = 0.95f;
const unsigned int barWidth = 12;
const unsigned int count = SCREENWIDTH / barWidth;

float values[count] = {};

sf::Clock iterationTimer;
float timeScale = 1.0f;

unsigned int sortingIterationsLeft;
unsigned int currentSortingStep;

unsigned int comparisons;
unsigned int arrayReads;

std::list<unsigned int> activeValues = {};

enum class Algorithm
{
    None,
    Confirm,
    FisherYatesShuffle,
    BubbleSort,
    SelectionSort,
    InsertionSort
};

Algorithm algorithm = Algorithm::None;
std::string algName = "";

void swap(float* a, float* b);

int minIndex(int i, int j);

bool checkIterationDelay(unsigned int delayInMilliseconds);

void startAlgorithm(Algorithm alg = Algorithm::None) {
    if (algorithm == alg) return;

    algorithm = alg;
    activeValues.clear();

    if (alg != Algorithm::Confirm) {
        comparisons = 0;
        arrayReads = 0;
    }
    
    switch (alg)
    {
    case Algorithm::None:
        algName = "";
        break;
    case Algorithm::Confirm:
        sortingIterationsLeft = count - 1;
        return;
    case Algorithm::FisherYatesShuffle:
        sortingIterationsLeft = count - 1;
        algName = "Fisher-Yates Shuffle";
        break;
    case Algorithm::BubbleSort:
        sortingIterationsLeft = count;
        currentSortingStep = 0;
        algName = "Bubble Sort";
        break;
    case Algorithm::SelectionSort:
        currentSortingStep = 0;
        algName = "Selection Sort";
        break;
    case Algorithm::InsertionSort:
        currentSortingStep = 1;
        algName = "Insertion Sort";
        break;
    default:
        break;
    }
    
    if (!algName.empty()) std::cout << "[Starting] " + algName << std::endl;
}

bool confirm() {
    if (sortingIterationsLeft < 1) {
        if (!checkIterationDelay(1000)) return false;
        std::cout << "\t[Result] " + std::to_string(comparisons) + " comparisons and " + std::to_string(arrayReads) + " array reads were made\n" << std::endl;
        return true;
    }

    if (!checkIterationDelay(10)) return false;

    if (values[count - sortingIterationsLeft - 1] < values[count - sortingIterationsLeft]) {
        activeValues.push_back(count - sortingIterationsLeft - 1);
        activeValues.push_back(count - sortingIterationsLeft);

        sortingIterationsLeft--;
        return false;
    }
    else
        std::cout << "Sorting Error" << std::endl;
}

bool randomize()
{
    if (!checkIterationDelay(10)) return false;

    if (sortingIterationsLeft == 0) return true;

    algorithm = Algorithm::FisherYatesShuffle;

    srand(time(NULL));

    // Pick a random index from 0 to i
    unsigned int j = rand() % (sortingIterationsLeft + 1);

    activeValues = { sortingIterationsLeft, j };
    // Swap arr[i] with the element at random index
    swap(&values[sortingIterationsLeft], &values[j]);
    
    sortingIterationsLeft--;
    return false;
}

bool bubbleSort() {
    if (!checkIterationDelay(1)) return false;
    
    if (sortingIterationsLeft == 1 ) return true;

    if (currentSortingStep >= sortingIterationsLeft - 1) {
        (sortingIterationsLeft)--;
        currentSortingStep = 0;
        return false;
    }

    activeValues = { currentSortingStep, currentSortingStep + 1 };

    arrayReads += 2;
    if (values[currentSortingStep] > values[currentSortingStep + 1])
        swap(&values[currentSortingStep], &values[currentSortingStep + 1]);

    comparisons++;
    currentSortingStep++;
    return false;
}

bool selectionSort() {
    if (!checkIterationDelay(50)) return false;

    if (currentSortingStep == count) return true;

    // calling minimum index function for minimum index
    unsigned int k = minIndex(currentSortingStep, count - 1);

    activeValues = { k, currentSortingStep };
    // Swapping when index and minimum index are not same
    if (k != currentSortingStep)
        swap(&values[k], &values[currentSortingStep]);

    currentSortingStep++;
    return false;
}

bool insertionSort() {
    if (!checkIterationDelay(35)) return false;

    if (currentSortingStep == count) return true;

    comparisons += 2;
    float key = values[currentSortingStep];
    unsigned int j = currentSortingStep - 1;

    while (j >= 0 && values[j] > key)
    {
        comparisons += 2;
        arrayReads += 3;
        values[j + 1] = values[j];
        j = j - 1;
    }
    values[j + 1] = key;

    arrayReads += 3;
    activeValues = { j + 1, currentSortingStep };

    currentSortingStep++;
    return false;
}

int main()
{
    // Window setup
    sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Sorting Visualization");

    if (SCREENWIDTH % barWidth != 0) std::cout << "WARNING: Width of window not divisible by width of bars (" + std::to_string(SCREENWIDTH% barWidth) + "px unused)" << std::endl;

    std::cout << "Controls\n\n'C' - Cancel current algorithm\n'R' - Fisher-Yates Shuffle\n'B' - Bubble Sort\n'S' - Selection Sort\n'I' - Insertion Sort\n\n" << std::endl;

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Equally distribute values along screen width
    for (int i = 0; i < count; i++)
    {   
        values[i] = SCREENHEIGHT * barHeightScale / count * ((double)i+1);
    }

    // Randomize values
    startAlgorithm(Algorithm::FisherYatesShuffle);

    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Clear screen
        window.clear();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        {
            startAlgorithm(Algorithm::None);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            startAlgorithm(Algorithm::FisherYatesShuffle);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        {
            startAlgorithm(Algorithm::BubbleSort);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            startAlgorithm(Algorithm::SelectionSort);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        {
            startAlgorithm(Algorithm::InsertionSort);
        }

        // TODO: Change time scale

        switch (algorithm)
        {
        case Algorithm::None:
            break;
        case Algorithm::Confirm:
            if (confirm()) startAlgorithm(Algorithm::FisherYatesShuffle);
            break;
        case Algorithm::FisherYatesShuffle:
            if (randomize()) startAlgorithm();
            break;
        case Algorithm::BubbleSort:
            if (bubbleSort()) startAlgorithm(Algorithm::Confirm);
            break;
        case Algorithm::SelectionSort:
            if (selectionSort()) startAlgorithm(Algorithm::Confirm);
            break;
        case Algorithm::InsertionSort:
            if (insertionSort()) startAlgorithm(Algorithm::Confirm);
            break;
        default:
            break;
        }

        // Draw calls
        for (int i = 0; i < count; i++)
        {
            sf::RectangleShape value(sf::Vector2f(barWidth, -values[i]));
            value.setPosition(barWidth * i, SCREENHEIGHT);

            if (std::find(activeValues.begin(), activeValues.end(), i) != activeValues.end())
                if (algorithm == Algorithm::Confirm)
                    value.setFillColor(sf::Color::Green);
                else
                    value.setFillColor(sf::Color::Red);
            else
                value.setFillColor(sf::Color::White);

            value.setOutlineThickness(1.0f);
            value.setOutlineColor(sf::Color::Black);
            window.draw(value);
        }

        if (!algName.empty()) {
            std::string details = algName + " - " + std::to_string(comparisons) + " comparisons, " + std::to_string(arrayReads) + " array reads";
            sf::Text info = sf::Text(details, font);
            window.draw(info);
        }

        // Update the window
        window.display();
    }
    return EXIT_SUCCESS;
}

void swap(float* a, float* b)
{
    arrayReads += 2;

    float temp = *a;
    *a = *b;
    *b = temp;
}

int minIndex(int i, int j)
{
    if (i == j)
        return i;

    // Find minimum of remaining elements
    int k = minIndex(i + 1, j);

    arrayReads += 2;
    comparisons++;
    // Return minimum of current and remaining.
    return (values[i] < values[k]) ? i : k;
}

bool checkIterationDelay(unsigned int delayInMilliseconds)
{
    if (iterationTimer.getElapsedTime().asMilliseconds() < delayInMilliseconds * timeScale) return false;

    iterationTimer.restart();
    return true;
}