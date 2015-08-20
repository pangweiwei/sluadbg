// The MIT License (MIT)

// Copyright 2015 Siney/Pangweiwei siney@yeah.net
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef DEBUGCONSOLE_H
#define DEBUGCONSOLE_H

#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QStack>

class DebugConsole : public QPlainTextEdit
{
  Q_OBJECT

public:
  DebugConsole(QWidget *parent = 0);

  QString prompt() const;
  void setPrompt(const QString &prompt);

protected:
  void keyPressEvent(QKeyEvent *e);

  void mousePressEvent(QMouseEvent *)       { /* Ignore */ }
  void mouseDoubleClickEvent(QMouseEvent *) { /* Ignore */ }
  void mouseMoveEvent(QMouseEvent *)        { /* Ignore */ }
  void mouseReleaseEvent(QMouseEvent *)     { /* Ignore */ }

private:
  void handleLeft(QKeyEvent *event);
  void handleEnter();
  void handleHistoryUp();
  void handleHistoryDown();
  void handleHome();

  void moveToEndOfLine();
  void clearLine();
  QString getCommand() const;

  int getIndex (const QTextCursor &crQTextCursor );

  QString userPrompt;
  QStack<QString> historyUp;
  QStack<QString> historyDown;
  bool locked, historySkip;

signals:
  void command(QString command);

public slots:
  void result(QString result);
  void append(QString text);
  void replace(QString text);
};

#endif
