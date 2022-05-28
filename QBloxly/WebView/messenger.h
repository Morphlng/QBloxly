#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>

class Messenger : public QObject
{
    Q_OBJECT
public:
    explicit Messenger(QObject *parent = nullptr);

signals:
    void blockChanged(const QString &text);
    void xmlGenerated(const QString &text);
    void triggerSave();
    void triggerLoadInput();
    void triggerLoadFile(const QString& file_content);
    void clearTrash();

public slots:
    // js向Qt传输，函数需要为slot
    void receiveCode(const QString &text) { emit blockChanged(text); }
    void receiveXML(const QString &text) { emit xmlGenerated(text); }
};

#endif // MESSENGER_H
