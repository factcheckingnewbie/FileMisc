#include "FilePanel.h"
#include <QInputDialog>
#include <KFileItem>
#include <KIO/CopyJob>
#include <KIO/StatJob>
#include <KI18n/KLocalizedString>

FilePanel::FilePanel(QWidget *parent)
    : QWidget(parent),
      m_model(new QFileSystemModel(this)),
      m_view(new QListView(this))
{
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);

    connect(m_view, &QListView::activated, this, &FilePanel::onActivated);
    connect(m_view, &QListView::customContextMenuRequested, this, &FilePanel::onCustomContextMenu);

    setLayout(layout);
}

void FilePanel::setDirectory(const QUrl &url)
{
    m_currentDir = url;
    m_model->setRootPath(url.toLocalFile());
    m_view->setRootIndex(m_model->index(url.toLocalFile()));
    emit directoryChanged(url);
}

QUrl FilePanel::currentDirectory() const
{
    return m_currentDir;
}

QUrl FilePanel::selectedUrl() const
{
    QModelIndex idx = m_view->currentIndex();
    if (!idx.isValid())
        return {};
    QString fileName = m_model->filePath(idx);
    return QUrl::fromLocalFile(fileName);
}

void FilePanel::refresh()
{
    setDirectory(m_currentDir);
}

void FilePanel::onActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QFileInfo info(m_model->filePath(index));
    if (info.isDir()) {
        setDirectory(QUrl::fromLocalFile(info.absoluteFilePath()));
    }
}

void FilePanel::onCustomContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    QAction *copyAct = menu.addAction(tr("Copy"));
    QAction *deleteAct = menu.addAction(tr("Delete"));
    QAction *renameAct = menu.addAction(tr("Rename"));
    QAction *chosen = menu.exec(m_view->viewport()->mapToGlobal(pos));
    if (chosen == copyAct)
        onCopyFile();
    else if (chosen == deleteAct)
        onDeleteFile();
    else if (chosen == renameAct)
        onRenameFile();
}

void FilePanel::onDeleteFile()
{
    QUrl url = selectedUrl();
    if (!url.isValid())
        return;
    KIO::DeleteJob *job = KIO::del(url, KIO::HideProgressInfo);
    connect(job, &KJob::result, this, [=](KJob *job) {
        if (job->error()) {
            showError(job->errorString());
        } else {
            refresh();
        }
    });
}

void FilePanel::onCopyFile()
{
    QUrl src = selectedUrl();
    if (!src.isValid())
        return;
    QString destDir = QInputDialog::getText(this, tr("Copy to..."), tr("Destination directory:"),
                                            QLineEdit::Normal, m_currentDir.toLocalFile());
    if (destDir.isEmpty())
        return;
    QUrl destUrl = QUrl::fromLocalFile(destDir);
    KIO::CopyJob *job = KIO::copy(src, destUrl, KIO::HideProgressInfo);
    connect(job, &KJob::result, this, [=](KJob *job) {
        if (job->error()) {
            showError(job->errorString());
        } else {
            refresh();
        }
    });
}

void FilePanel::onRenameFile()
{
    QUrl src = selectedUrl();
    if (!src.isValid())
        return;
    QString newName = QInputDialog::getText(this, tr("Rename"), tr("New Name:"), QLineEdit::Normal, src.fileName());
    if (newName.isEmpty())
        return;
    QUrl dest = src.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash);
    dest.setPath(dest.path() + "/" + newName);
    KIO::Job *job = KIO::move(src, dest, KIO::HideProgressInfo);
    connect(job, &KJob::result, this, [=](KJob *job) {
        if (job->error()) {
            showError(job->errorString());
        } else {
            refresh();
        }
    });
}

void FilePanel::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace) {
        QDir dir(m_currentDir.toLocalFile());
        if (dir.cdUp())
            setDirectory(QUrl::fromLocalFile(dir.absolutePath()));
    }
    QWidget::keyPressEvent(event);
}

void FilePanel::showError(const QString &msg)
{
    QMessageBox::critical(this, tr("Error"), msg);
}
