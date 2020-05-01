#include<set>
#include<iostream>
#include<utility>
#include<vector>
#include<tuple>
#include<thread>
#include<Windows.h>
#include<future>
#define ZERO 0
#define Element sudokuPuzzle.at(CurrentRow).at(Column)

const int MaximumDigit = 81;
const int MaxHeight{ 8 }; // If The current Row Reached this
const int Minimum{ 2 };
const int IncrementByThree{ 3 };

typedef std::vector<std::vector<short>> SudokuPuzzle;
typedef const std::vector<std::vector<short>>& ConstSudokuRef;
typedef std::vector < std::tuple<int, int, int>> UnAssignedCoordinates;
void PrintSudokoPuzzle(ConstSudokuRef sudokuPuzzle)
{
	for (size_t y{ 0 }; y <= 8; ++y) {
		for (size_t x{ 0 }; x <= 9; ++x) {
			while (x != 9) {
				std::cout << sudokuPuzzle.at(y).at(x) << ' ';
				break;
			}
		}
		std::cout << '\n';
	}
}
int GeneratePoint(int coordinate) { // Return the corresponding location to where i can generate a grid for the specialized function
	if (coordinate >= 0 && coordinate <= 2)
		return 0;
	else if (coordinate >= 3 && coordinate <= 5)
		return 3;
	else
		return 6;
}
int GenerateMax(int coordinate) {
	if (coordinate == 0)
		return 2;
	else if (coordinate == 3)
		return 5;
	else
		return 8;
}
UnAssignedCoordinates GenerateCoordinatesByColumn(ConstSudokuRef sudokuPuzzle) {
	UnAssignedCoordinates Coordinates;
	int digitcounter{ 0 };
	int MaxRow{ 2 };
	int MaxColumn{ 2 };
	int CurrentRow{ 0 };
	while (digitcounter < MaximumDigit) { // Main Termination Condition // if everything is ok this terminates
		while (CurrentRow <= MaxHeight) {
			for (CurrentRow; CurrentRow <= MaxRow; ++CurrentRow) {
				for (int Column{ 0 }; Column <= MaxHeight; ++Column) {
					if (Element == 0) {
						Coordinates.emplace_back(std::make_tuple(CurrentRow, Column , 0));
					}
					++digitcounter;
				}
			}
			MaxRow += IncrementByThree;
		}
		CurrentRow = ZERO;
		MaxRow = Minimum;
		MaxColumn += IncrementByThree;

	}
	return Coordinates;
}
std::set<int> DigitsPresentInRow(ConstSudokuRef puzzle, int CurrentRow, int CurrentColumn) {
	std::set<int> unique;
	for (int x{ 0 }; x <= MaxHeight; ++x) {
		if (puzzle.at(CurrentRow).at(x) != 0) {
			unique.insert(puzzle.at(CurrentRow).at(x));
		}
	}
	return unique;
}
std::set<int> DigitsPresentInColumn(ConstSudokuRef puzzle, int CurrentRow, int CurrentColumn) {
	std::set<int> unique;
	for (int x{ 0 }; x <= MaxHeight; ++x) {
		if (puzzle.at(x).at(CurrentColumn) != 0) {
			unique.insert(puzzle.at(x).at(CurrentColumn));
		}
	}
	return unique;
}
std::set<int> DigitsPresentInSubGrid(ConstSudokuRef puzzle, int	PCurrentRow, int PColumn) {
	std::set<int> UniqueNum;
	int CurrentRow = GeneratePoint(PCurrentRow);
	int MaxRow = GenerateMax(CurrentRow);
	int Column = GeneratePoint(PColumn);
	int MaxColumn = GenerateMax(Column);

	int CurrentColumn{ Column };
	for (CurrentRow; CurrentRow <= MaxRow; ++CurrentRow) {
		for (Column; Column <= MaxColumn; ++Column) {
			if (puzzle.at(CurrentRow).at(Column) != 0)
				UniqueNum.insert(puzzle.at(CurrentRow).at(Column));
		}
		Column = CurrentColumn;
	}

	return UniqueNum;
}

int main() {
	
	SudokuPuzzle sudokuPuzzle // 2d Vector to hold the sudoku puzzle
	{
		 {0, 4, 0, 9, 0, 0, 0, 0, 7},
		 {1, 9, 0, 6, 0, 0, 0, 0, 4},
		 {5, 0, 0, 0, 0, 0, 0, 1, 0},
		
		 {0, 8, 0, 0, 3, 0, 0, 7, 0},
		 {2, 0, 0, 0, 0, 4, 5, 0, 8},
	     {0, 0, 0, 5, 0, 0, 0, 0, 0},
		 
		 {0, 0, 0, 0, 0, 0, 0, 2, 0},
		 {0, 0, 0, 0, 0, 0, 3, 4, 0},
		 {0, 7, 0, 0, 0, 6, 0, 0, 1}
	};
	std::future<UnAssignedCoordinates> task1 = std::async(std::launch::async, GenerateCoordinatesByColumn, sudokuPuzzle);
	UnAssignedCoordinates Coordinates = task1.get();
	auto CoordinateIterator = Coordinates.begin(); 
	std::set<int> NumbersInSubGrid, NumbersInRow, NumbersInColumn;
	bool IsInserted = false;
	
	while (CoordinateIterator != Coordinates.end()) {
		std::future<std::set<int>> Task1 = std::async(std::launch::async, DigitsPresentInSubGrid, sudokuPuzzle, std::get<0>(*CoordinateIterator), std::get<1>(*CoordinateIterator));
		std::future<std::set<int>> Task2 = std::async(std::launch::async, DigitsPresentInRow, sudokuPuzzle, std::get<0>(*CoordinateIterator), std::get<1>(*CoordinateIterator));
		std::future<std::set<int>> Task3 = std::async(std::launch::async, DigitsPresentInColumn, sudokuPuzzle, std::get<0>(*CoordinateIterator), std::get<1>(*CoordinateIterator));
		NumbersInSubGrid = Task1.get();
		NumbersInRow = Task2.get();
		NumbersInColumn = Task3.get();
		IsInserted = false;
		
		for (int PossibleNumber{ std::get<2>(*CoordinateIterator) }; PossibleNumber <= 9;) {
			++PossibleNumber;
			if (PossibleNumber > 9) {
				break;
			}
			
			if ((NumbersInSubGrid.insert(PossibleNumber).second)
				&& (NumbersInRow.insert(PossibleNumber).second)
				&& (NumbersInColumn.insert(PossibleNumber).second))

			{
				sudokuPuzzle.at(std::get<0>(*CoordinateIterator)).at(std::get<1>(*CoordinateIterator)) = PossibleNumber;
				std::get<2>(*CoordinateIterator) = PossibleNumber;
				IsInserted = true;
				break;
			}
		}

		if(IsInserted)
			++CoordinateIterator;
		else {
			std::get<2>(*CoordinateIterator) = 0;
			sudokuPuzzle.at(std::get<0>(*CoordinateIterator)).at(std::get<1>(*CoordinateIterator)) = 0;
			--CoordinateIterator;
		}
		/*PrintSudokoPuzzle(sudokuPuzzle);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		system("CLS");*/
	}



	PrintSudokoPuzzle(sudokuPuzzle);

	return 0;
}