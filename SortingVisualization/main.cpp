#include <SFML/Graphics.hpp>

#include <iostream>
#include <list>

const unsigned int SCREENWIDTH = 1000;
const unsigned int SCREENHEIGHT = 600;

const float barHeightScale = 0.95f;
const unsigned int barWidth = 10;
const unsigned int count = SCREENWIDTH / barWidth;

float values[count] = {};

sf::Clock iterationTimer;
float timeScale = 1.0f;

unsigned int sortingIterationsLeft;
unsigned int currentSortingStep;

unsigned int comparisons;
unsigned int arrayReads;

std::list<unsigned int> activeValues = {};

enum Algorithm
{
    None,
    Confirm,
    FisherYatesShuffle,
    BubbleSort
};

Algorithm algorithm = Algorithm::None;

void swap(float* a, float* b);

bool checkIterationDelay(unsigned int delayInMilliseconds);

void startAlgorithm(Algorithm alg = Algorithm::None) {
    if (algorithm == alg) return;

    if (alg != Algorithm::Confirm) {
        comparisons = 0;
        arrayReads = 0;
    }
    
    switch (alg)
    {
    case None:
        algorithm = Algorithm::None;
        activeValues.clear();
        break;
    case Confirm:
        algorithm = Algorithm::Confirm;
        sortingIterationsLeft = count - 1;
        break;
    case FisherYatesShuffle:
        algorithm = Algorithm::FisherYatesShuffle;
        sortingIterationsLeft = count - 1;
        break;
    case BubbleSort:
        algorithm = Algorithm::BubbleSort;
        sortingIterationsLeft = count;
        currentSortingStep = 0;
        break;
    default:
        break;
    }
}

bool confirm() {
    if (sortingIterationsLeft < 1) {
        if (!checkIterationDelay(3000)) return false;
        activeValues.clear();
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

    if (sortingIterationsLeft < 1) {
        activeValues.clear();
        return true;
    }

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
    
    if (sortingIterationsLeft == 1 ) {
        activeValues.clear();
        return true;
    }

    if (currentSortingStep >= sortingIterationsLeft - 1) {
        (sortingIterationsLeft)--;
        currentSortingStep = 0;
        return false;
    }

    activeValues = { currentSortingStep, currentSortingStep + 1 };

    arrayReads += 2;
    if (values[currentSortingStep] > values[currentSortingStep + 1]) {
        swap(&values[currentSortingStep], &values[currentSortingStep + 1]);
    }

    comparisons++;
    currentSortingStep++;
    return false;
}

int main()
{
    // Window setup
    sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Sorting Visualization");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cout << "Couldn't load font" << std::endl;
    }

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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            startAlgorithm(Algorithm::FisherYatesShuffle);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
        {
            startAlgorithm(Algorithm::BubbleSort);
        }

        // TODO: Change time scale

        switch (algorithm)
        {
        case None:
            break;
        case Confirm:
            if (confirm()) startAlgorithm();
            break;
        case FisherYatesShuffle:
            if (randomize()) startAlgorithm();
            break;
        case BubbleSort:
            if (bubbleSort()) startAlgorithm(Algorithm::Confirm);
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

        std::string details = std::to_string(comparisons) + " comparisons, " + std::to_string(arrayReads)+ " array reads";
        sf::Text info = sf::Text(details, font);
        window.draw(info);

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

bool checkIterationDelay(unsigned int delayInMilliseconds)
{
    if (iterationTimer.getElapsedTime().asMilliseconds() < delayInMilliseconds * timeScale) return false;

    iterationTimer.restart();
    return true;
}