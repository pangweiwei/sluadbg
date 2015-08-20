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

#include "debugconsole.h"
#include <iostream>

DebugConsole::DebugConsole(QWidget *parent) : QPlainTextEdit(parent),
  userPrompt(QString("> ")),
  locked(false),
  historySkip(false)
{
  historyUp.clear();
  historyDown.clear();
  setLineWrapMode(NoWrap);
  insertPlainText(userPrompt);
}

void DebugConsole::keyPressEvent(QKeyEvent *e) {
  if(locked) return;


  switch(e->key()) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    handleEnter();
    break;
  case Qt::Key_Backspace:
    handleLeft(e);
    break;
  case Qt::Key_Up:
    handleHistoryUp();
    break;
  case Qt::Key_Down:
    handleHistoryDown();
    break;
  case Qt::Key_Left:
    handleLeft(e);
    break;
  case Qt::Key_Home:
    handleHome();
    break;
  default:
    QPlainTextEdit::keyPressEvent(e);
    break;
  }
}

// Enter key pressed
void DebugConsole::handleEnter() {
  QString cmd = getCommand();

  if(0 < cmd.length()) {
    while(historyDown.count() > 0) {
      historyUp.push(historyDown.pop());
    }

    historyUp.push(cmd);
  }

  moveToEndOfLine();

  if(cmd.length() > 0) {
    locked = true;
    setFocus();
    insertPlainText("\n");
    emit command(cmd);
  } else {
    insertPlainText("\n");
    insertPlainText(userPrompt);
    ensureCursorVisible();
  }
}

void DebugConsole::result(QString result) {
  insertPlainText(result);
  insertPlainText("\n");
  insertPlainText(userPrompt);
  ensureCursorVisible();
  locked = false;
}

// Append line but do not display prompt afterwards
void DebugConsole::append(QString text) {
  insertPlainText(text);
  ensureCursorVisible();
}

void DebugConsole::replace(QString text)
{
    QTextCursor c = this->textCursor();
    c.movePosition(QTextCursor::StartOfLine);
    c.select(QTextCursor::LineUnderCursor);
    c.removeSelectedText();
    append(text);
}

void DebugConsole::handleHistoryUp() {
  if(0 < historyUp.count()) {
    QString cmd = historyUp.pop();
    historyDown.push(cmd);

    clearLine();
    insertPlainText(cmd);
  }

  historySkip = true;
}

void DebugConsole::handleHistoryDown() {
  if(0 < historyDown.count() && historySkip) {
    historyUp.push(historyDown.pop());
    historySkip = false;
  }

  if(0 < historyDown.count()) {
    QString cmd = historyDown.pop();
    historyUp.push(cmd);

    clearLine();
    insertPlainText(cmd);
  } else {
    clearLine();
  }
}

void DebugConsole::clearLine() {
  QTextCursor c = this->textCursor();
  c.select(QTextCursor::LineUnderCursor);
  c.removeSelectedText();
  this->insertPlainText(userPrompt);
}

QString DebugConsole::getCommand() const {
  QTextCursor c = this->textCursor();
  c.select(QTextCursor::LineUnderCursor);

  QString text = c.selectedText();
  text.remove(0, userPrompt.length());

  return text;
}

void DebugConsole::moveToEndOfLine() {
  QPlainTextEdit::moveCursor(QTextCursor::EndOfLine);
}

void DebugConsole::handleLeft(QKeyEvent *event) {
  if(getIndex(textCursor()) > userPrompt.length()) {
    QPlainTextEdit::keyPressEvent(event);
  }
}

void DebugConsole::handleHome() {
  QTextCursor c = textCursor();
  c.movePosition(QTextCursor::StartOfLine);
  c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, userPrompt.length());
  setTextCursor(c);
}

int DebugConsole::getIndex (const QTextCursor &crQTextCursor ) {
  QTextBlock b;
  int column = 1;
  b = crQTextCursor.block();
  column = crQTextCursor.position() - b.position();
  return column;
}

void DebugConsole::setPrompt(const QString &prompt) {
  userPrompt = prompt;
  clearLine();
  locked = false;
}

QString DebugConsole::prompt() const {
  return userPrompt;
}
