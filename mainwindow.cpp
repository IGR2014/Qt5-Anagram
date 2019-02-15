#include <QDir>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QTextCodec>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  ui(new Ui::MainWindow),
										  derivationIndices(NULL) {

	ui->setupUi(this);

	// File with words - a.k.a dictionary
	QFile file(QDir::currentPath() + "/words.txt");

	// Open file in text mode for reading
	if (file.open(QIODevice::ReadOnly |
				  QIODevice::Text)) {

		// Read data from file
		QString data = QString::fromUtf8(file.readAll());
		// Split word wich separated with "\n"
		words = data.split("\n");

		derivationIndices = new int[words.count()];
		memset(derivationIndices, -2, words.count() * sizeof(int));

		// Go through all words
		for (int i = 0; i < words.count(); ++i) {

			// Insert word into list of all words
			ui->plainTextEdit->insertPlainText(words[i]);

			// Visual formatting
			if (i < words.count() - 1) {

				// Insert coma between words
				ui->plainTextEdit->insertPlainText(", ");

			} else {

				// Insert dot at the end of words list
				ui->plainTextEdit->insertPlainText(".");

			}

			// If word is made of 3 letters - it is one of top level (root) words
			if (words[i].length() == 3) {

				// Show it in combo box
				ui->comboBox->addItem(words[i]);
				// Insert word number to list of 3-letter word numbers
				keyWords.append(i);

				// Set current word as top level (root)
				derivationIndices[i] = -1;

			}

			// Run derivations search for current word
			searchAnagramDerivations(words[i], i);

		}

		// Set index to zero
		ui->comboBox->setCurrentIndex(-1);

		// Connect combo box index chang signal with anagram search
		connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(getLongestDerivationForWord(int)));

	} else {

		// Oops!
		// File system troubles with file
		QMessageBox mbs;
		mbs.setText("Unable to read file!");
		mbs.exec();

	}

}

// Derivations search function.
// Allows to search derivations of word in all words list
// Firs parameter - word
// Second parameter - word's index
void MainWindow::searchAnagramDerivations(QString &word, int &index) {

	// Go through all words to find anagrams
	for (int j = 0; j < words.count(); ++j) {

		// Assume current word contain N letters
		// So we are looking for words with N + 1 letters
		if (words[j].length() == (word.length() + 1)) {

			// Counter of same letters in two words
			int sameLettersCount = 0;

			// Comapre to word to calculate number of same letters
			// Going through all letters of word with N letters
			for (int k = 0; k < word.length(); ++k) {

				// And same for word with N + 1 letters
				for (int l = 0; l < words[j].length(); ++l) {

					// If we found same letter
					if (words[j].data()[l] == word.data()[k]) {

						// Increment counter
						sameLettersCount++;
						// And move to next letter
						// This break is really important, because
						// without it, e.g. "dog" and "door" will be
						// interpreted by alghoritm as anagram derivation
						// which is incorrect!
						break;

					}

				}

			}

			// If counter of same letters for two words
			// equals to letter count of shorter word
			// than N + 1 letters word is anagram derivation
			// of N letters word
			if (sameLettersCount == word.length()) {

				derivationIndices[j] = index;

			}

			// Finally, reset our counter
			sameLettersCount = 0;

		}

	}

}

// Derivations chains recursive search
// Allows to recursive search derivations chain and measure their depth
// First parameter - index of last element in chain
// Second parameter - chain depth (count fo elements till top level)
int MainWindow::searchDerivationChain(int &index, int &count) {

	// Index of next word for which current
	// word is anagram derivation
	int nextDerivationNumber = derivationIndices[index];

	// Index of top level (root) word number
	int rootNumber = -1;

	// If next word index is not -2 (word is not derivation and not top level)
	// or -1 (word is top level which means it is not derivation)
	if (nextDerivationNumber > -1) {

		// Recursively search words in depth
		// direction (from derivation to root)
		rootNumber = searchDerivationChain(nextDerivationNumber, ++count);

	// If next word does not exists
	// then current is top level (root)
	} else if (nextDerivationNumber == -1) {

		// Save top level (root) word number
		rootNumber = index;

	}

	// Return top level (root) word number
	return rootNumber;

}

// Search longest derivations chain and return last derivation number
// First parameter - number of top level (root) word
int MainWindow::longestDerivationChain(int root) {

	// Number of last word in chain
	int lastElementNumber = -1;
	// Number of words in longest chain
	int count = 0;

	// Go through all words
	for (int i = 0; i < words.count(); ++i) {

		// Current depth of derivation
		int currentCount = 0;
		// Number of top level (root) woord for current derivation
		int currentRoot = searchDerivationChain(i, currentCount);

		// If current root is what we are looking for
		// and depth is bigger than number of words
		// in longest chain than current chain is now
		// the longest one
		if (currentRoot == root &&
			currentCount > count) {

			count = currentCount;
			lastElementNumber = i;

		}

	}

	// Return number of last derivation element
	return lastElementNumber;

}

// Get derivation for 3-letter word from combo box
void MainWindow::getLongestDerivationForWord(int word) {

	// List of words
	QList<int> tempOut;

	// Clear previous info
	ui->lineEdit->clear();

	// Get index of last word of longest
	// chain for currently choosed word
	int longest = longestDerivationChain(keyWords[word]);

	// Add last word to list
	tempOut.append(longest);

	// Check if chain exists
	if (longest > -2) {

		// Reversing chain direction
		// (before: from last to root
		// after - from root to last)
		while (longest > -1) {

			tempOut.insert(0, derivationIndices[longest]);
			longest = derivationIndices[longest];

		}

		// Delete one element because it is "-1"
		// and there is no such index in list of words
		tempOut.pop_front();

		// Output anagram derivations for choosen word one by one
		for (int i = 0; i < tempOut.count(); ++i) {

			// Output next word
			ui->lineEdit->insert(words[tempOut[i]]);

			// Output " -> " after X - 1 words
			// in line of X words
			if (i < tempOut.count() - 1) {

				ui->lineEdit->insert(QString(" -> "));

			}

		}

	}

}

MainWindow::~MainWindow() {

	delete ui;

	// Cleanup
	if (derivationIndices) {

		delete [] derivationIndices;

	}

}
