#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

class Editor : public QMainWindow {
    Q_OBJECT
public:
    Editor() {
        textEdit = new QTextEdit(this);
        setCentralWidget(textEdit);

        QMenu *fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&Open", this, &Editor::openFile, QKeySequence::Open);
        fileMenu->addAction("&Save", this, &Editor::saveFile, QKeySequence::Save);
    }

private slots:
    void openFile() {
        QString path = QFileDialog::getOpenFileName(this, "Open File");
        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textEdit->setPlainText(file.readAll());
            }
        }
    }

    void saveFile() {
        QString path = QFileDialog::getSaveFileName(this, "Save File");
        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(textEdit->toPlainText().toUtf8());
            }
        }
    }

private:
    QTextEdit *textEdit;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Editor editor;
    editor.resize(800, 600);
    editor.show();
    return app.exec();
}