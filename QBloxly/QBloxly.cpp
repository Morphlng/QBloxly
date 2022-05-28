#include "QBloxly.h"
#include <QTimer>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include "Cxx/Runner.h"

QBloxly::QBloxly(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    // IO redirect
    m_cout = new ThreadLogStream(std::cout, this);
    m_cerr = new ThreadLogStream(std::cerr, this);
    msgHandler = new MessageHandler(ui.console, this);
    connect(m_cout, &ThreadLogStream::sendLogString, msgHandler, &MessageHandler::catchMessage);
    connect(m_cerr, &ThreadLogStream::sendLogString, msgHandler, &MessageHandler::catchError);

    // Console init
    cxx_session = new ScriptSession(ui.console);
    connect(cxx_session, &ScriptSession::clearConsoleOutput, ui.console, &QConsoleWidget::clear);
    QTimer::singleShot(100, cxx_session, SLOT(REPL()));
    // Console is hiding by default
    ui.console->hide(); 

    // WebView init
    QString strHtml("webfiles/index.html");
    ui.webView->load(QUrl("qrc:/assets/" + strHtml));
    webChannel = new QWebChannel(this);
    messenger = new Messenger(this);
    webChannel->registerObject("Messenger", messenger);

    // these are javascript sending message to Qt
    connect(messenger, &Messenger::blockChanged, ui.textCode, &QPlainTextEdit::setPlainText);
    connect(messenger, &Messenger::xmlGenerated, this, &QBloxly::saveBlocksAsXML);

    // these are Qt sending request to javascript
    connect(ui.actionSave, &QAction::triggered, messenger, &Messenger::triggerSave);
    connect(ui.actionLoadXML, &QAction::triggered, messenger, &Messenger::triggerLoadInput);
    connect(ui.actionLoadFile, &QAction::triggered, this, &QBloxly::readFileToXML);
    connect(ui.actionClearTrash, &QAction::triggered, messenger, &Messenger::clearTrash);

    // these are UI related
    connect(ui.actionToggleTerminal, &QAction::triggered, this, &QBloxly::toggleConsole);
    connect(ui.btnRun, &QPushButton::clicked, this, &QBloxly::runBlock);
    connect(ui.actionZoomIn, &QAction::triggered, this, &QBloxly::zoomInText);
    connect(ui.actionZoomOut, &QAction::triggered, this, &QBloxly::zoomOutText);
}

void QBloxly::toggleConsole()
{
    if (isConsoleOpened)
    {
        isConsoleOpened = false;
        ui.console->hide();
        ui.actionToggleTerminal->setText("打开终端");
    }
    else
    {
        isConsoleOpened = true;
        ui.console->show();
        ui.actionToggleTerminal->setText("关闭终端");
    }
}

void QBloxly::saveBlocksAsXML(const QString& xml_text)
{
    QString file_path = QFileDialog::getSaveFileName(this,
        "Save Blocks",
        "./",
        "xml files (*.xml);;all files(*.*)");

    if (!file_path.isEmpty())
    {
        QFile file(file_path);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QMessageBox::critical(this,
                tr("Error"),
                tr("Failed to open file \"%1\" for save!").arg(file_path),
                QMessageBox::Ok);
        }
        file.write(xml_text.toUtf8());
        file.close();
    }
}

void QBloxly::readFileToXML()
{
    QString filename = QFileDialog::getOpenFileName(this, "选择文件", "./", "xml files (*.xml)");
    if (filename.isEmpty()) {
        return;
    }
    else {
        QFile file(filename);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString file_content(file.readAll());
        file.close();

        emit messenger->triggerLoadFile(file_content);
    }
}

void QBloxly::runBlock()
{
    QString blockCode = ui.textCode->toPlainText();
    if (!blockCode.isEmpty())
        cxx_session->runCode(blockCode);
}

void QBloxly::zoomInText()
{
    QFont font =  ui.textCode->font();
    font.setPointSize(font.pointSize() + 2);

    ui.textCode->setFont(font);
}

void QBloxly::zoomOutText()
{
    QFont font = ui.textCode->font();
    font.setPointSize(font.pointSize() - 2);

    ui.textCode->setFont(font);
}
