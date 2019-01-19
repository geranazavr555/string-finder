#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FileItem.h"
#include <QFileDialog>
#include <QDebug>
#include <QCommonStyle>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    index_thread(new QThread(this)),
    search_thread(new QThread(this)),
    index_worker(new IndexWorker()),
    searcher(),
    timer(),
    stage("Ready")
{
    qRegisterMetaType<size_t>("size_t");

    make_ui();
    make_connections();
    index_worker->moveToThread(index_thread);
    index_thread->start();
    search_thread->start();
}

MainWindow::~MainWindow()
{
    emit stop_search();
    emit stop_indexing();
    search_thread->quit();
    search_thread->wait();
    index_thread->requestInterruption();
    index_thread->quit();
    index_thread->wait();
    delete index_worker;
}

void MainWindow::make_ui()
{
    ui->setupUi(this);
    setWindowTitle("String searcher");

    QCommonStyle style;
    ui->actionOpenDirectory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionStop->setIcon(style.standardIcon(QCommonStyle::SP_DialogDiscardButton));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->progressBar->hide();
}

void MainWindow::make_connections()
{
    connect(ui->actionOpenDirectory, SIGNAL(triggered()), this, SLOT(open_directory()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(show_about_qt()));
    connect(ui->actionUsage, SIGNAL(triggered()), this, SLOT(show_help()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stop_clicked()));
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(search_init()));

    connect(this, &MainWindow::set_directory_to_search, index_worker, &IndexWorker::set_directory);
    connect(index_worker, &IndexWorker::files_counted, this, &MainWindow::set_steps_count);
    connect(index_worker, &IndexWorker::files_processed, this, &MainWindow::set_current_step);
    connect(index_worker, &IndexWorker::started, this, &MainWindow::indexing_started);
    connect(index_worker, &IndexWorker::finished, this, &MainWindow::indexing_finished);
    connect(this, &MainWindow::stop_indexing, index_worker, &IndexWorker::stop, Qt::DirectConnection);
}


void MainWindow::open_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select directory to index files", QDir::currentPath());

    if (!dir.isEmpty())
    {
        setWindowTitle("String search in " + dir);
        ui->treeWidget->clear();
        emit set_directory_to_search(dir);
    }
}

void MainWindow::indexing_finished()
{
    ui->statusBar->showMessage("Index built in " + QString::number(timer.elapsed()) + " ms");
    ui->progressBar->hide();
    set_stage("Ready");
}

void MainWindow::set_steps_count(size_t count)
{
    ui->progressBar->setMaximum(static_cast<int>(count));
    if (count > 0)
    {
        ui->statusBar->showMessage(stage + "... Files detected: " + QString::number(count));
    }
    else
    {
        ui->statusBar->showMessage(stage + "...");
    }
}

void MainWindow::indexing_started()
{
    timer.restart();
    ui->treeWidget->clear();
    ui->statusBar->showMessage("Indexing...");
    show_progress_bar();
    set_stage("Indexing");
}

void MainWindow::set_current_step(size_t step)
{
    ui->progressBar->setValue(static_cast<int>(step));
}

void MainWindow::show_about_qt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::show_help()
{
    QMessageBox::about(this, "About string finder", "Usage: choose the directory to find strings. "
                                                    "After indexing you will be able to search strings in text"
                                                    " files even if they are modifying\n\n"
                                                    "Georgiy Nazarov, github.com/geranazavr555");
}

void MainWindow::set_stage(QString const & new_stage)
{
    stage = new_stage;
}

void MainWindow::show_progress_bar()
{
    ui->progressBar->setMaximum(0);
    ui->progressBar->reset();
    ui->progressBar->show();
}

void MainWindow::searching_started()
{
    timer.restart();
    ui->treeWidget->clear();
    ui->statusBar->showMessage("Searching...");
    show_progress_bar();
    set_stage("Searching");
}

void MainWindow::searching_finished()
{
    ui->statusBar->showMessage("Searching finished in " + QString::number(timer.elapsed()) + " ms");
    ui->progressBar->hide();
    set_stage("Ready");
}

void MainWindow::search_init()
{
    if (!index_worker->has_index() || index_worker->is_indexing())
        return;

    QString pattern = ui->lineEdit->text();
    if (pattern.isEmpty())
    {
        ui->treeWidget->clear();
        return;
    }

    qDebug() << "search init: " << pattern;
    if (searcher)
        emit stop_search();
    searcher = index_worker->get_searcher(pattern);

    connect(this, &MainWindow::start_search, searcher.get(), &SearchEngine::start);
    connect(this, &MainWindow::stop_search, searcher.get(), &SearchEngine::stop, Qt::DirectConnection);
    connect(searcher.get(), &SearchEngine::started, this, &MainWindow::searching_started);
    connect(searcher.get(), &SearchEngine::finished, this, &MainWindow::searching_finished);
    connect(searcher.get(), &SearchEngine::files_count, this, &MainWindow::set_steps_count);
    connect(searcher.get(), &SearchEngine::files_processed, this, &MainWindow::set_current_step);
    connect(searcher.get(), &SearchEngine::found, this, &MainWindow::add_result);

    searcher->moveToThread(search_thread);
    emit start_search();
}

void MainWindow::add_result(const QString &file, const QString& slice)
{
    auto result_root_item = new QTreeWidgetItem(ui->treeWidget);
    result_root_item->setText(0, file);
    auto result_slice_item = new QTreeWidgetItem(result_root_item);
    result_slice_item->setText(0, slice);
    ui->treeWidget->addTopLevelItem(result_root_item);
}

void MainWindow::stop_clicked()
{
    qDebug() << "stop clicked";
    if (index_worker->is_indexing())
        emit stop_indexing();
    else
        emit stop_search();
}
