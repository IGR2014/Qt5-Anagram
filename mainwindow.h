#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>


namespace Ui {

	class MainWindow;

}

struct ListNode {

	QString* data;
	ListNode* topLevelNode;

};

class MainWindow : public QMainWindow {

		Q_OBJECT

	public:

		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		// Derivations search function.
		// Allows to search derivations of word in all words list
		// Firs parameter - word
		// Second parameter - word's index
		void searchAnagramDerivations(QString&, int&);

		// Derivations chains recursive search
		// Allows to recursive search derivations chain and measure their depth
		// First parameter - index of last element in chain
		// Second parameter - chain depth (count fo elements till top level)
		int searchDerivationChain(int&, int&);

		// Search longest derivations chain and return last derivation number
		// First parameter - number of top level (root) word
		int longestDerivationChain(int);


	public slots:

		// Get derivation for 3-letter word from combo box
		void getLongestDerivationForWord(int);


	private:

		Ui::MainWindow *ui;

		// List of words
		QStringList words;
		// List of derivation
		int* derivationIndices;
		// List of 3-letter word numbers
		QList<int> keyWords;

};

#endif // MAINWINDOW_H
