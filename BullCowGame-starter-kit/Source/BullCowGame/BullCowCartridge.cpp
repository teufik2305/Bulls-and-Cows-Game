// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay(); // Always stays on top

    const FString HiddenWordsListPath = FPaths::ProjectDir() / TEXT("Content/HiddenWordsList/HiddenWordsList.txt");
    FFileHelper::LoadFileToStringArray(HiddenWords, *HiddenWordsListPath);

    Isograms = GetValidWords(HiddenWords);
    SetupGame();
}

void UBullCowCartridge::OnInput(const FString &PlayerInput) // When the player hits enter
{
    if (bGameOver == true)
    {
        ClearScreen();
        SetupGame();
    }
    else // Checking Player Guess
    {
        ProcessGuess(PlayerInput);
    }
}

void UBullCowCartridge::SetupGame()
{
    // Welcoming the Player
    PrintLine(TEXT("Welcome to Bulls and Cows Guessing Game!"));

    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)];
    Lives = HiddenWord.Len();
    bGameOver = false;

    PrintLine(TEXT("Guess the %i letter word."), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives."), Lives);
    PrintLine(TEXT("Type in your guess then\npress enter to continue..."));

    PrintLine(TEXT("Showing HiddenWord for debugging..."));
    PrintLine(TEXT("This HiddenWord is: %s"), *HiddenWord);
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to play again!"));
}

void UBullCowCartridge::ProcessGuess(const FString &Guess)
{
    if (Guess == HiddenWord)
    {
        PrintLine(TEXT("Well done!\nYou guessed the word!"));
        EndGame();
        return;
    }

    // // Check if Isogram
    if (!IsIsogram(Guess))
    {
        PrintLine(TEXT("There are no repeating letters.\nGuess again!"));
        return;
    }

    // Check Right Number of Characters
    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("Wrong Guess. HiddenWord is %i letters long."), HiddenWord.Len());
        PrintLine(TEXT("Try guessing again!"));
        return;
    }

    // Remove the life
    if (Guess != HiddenWord)
    {
        PrintLine(TEXT("Lost Life!"));
        --Lives;
        PrintLine(TEXT("Wrong Guess. You have %i lives left!"), Lives);
    }

    if (Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("No more lives left. You have lost!"));
        PrintLine(TEXT("This HiddenWord was: %s"), *HiddenWord);
        EndGame();
        return;
    }

    // Show the player Bulls and Cows
    FBullCowCount Score = GetBullCows(Guess);
    PrintLine(TEXT("You have %i Bulls and %i Cows"), Score.Bulls, Score.Cows);
}

bool UBullCowCartridge::IsIsogram(const FString &Word) const
{
    /*  For each letter
    Start at element[0]
    Compare against the next letter
    Untill we reach [Word.Len() - 1] if Word.Len() = 4 we need our loop to stop at 3, Index < Word.Len() simply use this condition */
    // return false if any of the letter is equal/same to other
    // return true if not

    for (int32 Index = 0; Index < Word.Len(); Index++)
    {
        for (int32 Comparison = Index + 1; Comparison < Word.Len(); Comparison++)
        {
            if (Word[Index] == Word[Comparison])
            {
                return false;
            }
        }
    }

    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString> &WordList) const
{
    TArray<FString> ValidWords;

    for (FString Word : WordList)
    {
        if (Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word))
        {
            ValidWords.Emplace(Word);
        }
    }

    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString &Guess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            continue;
        }

        for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[HiddenIndex])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}