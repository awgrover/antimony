#include "ui/script_window.h"
#include "ui_main_window.h"

#include "app/app.h"
#include "ui/script/script_pane.h"
#include "ui/script/editor.h"

#include "graph/script_node.h"
#include "graph/graph.h"

ScriptWindow::ScriptWindow(ScriptNode* n)
    : graph(n->parentGraph()), pane(new ScriptPane(n, this))
{
    ui->menuView->deleteLater();
    ui->menuAdd->deleteLater();

    connect(ui->actionCopy, &QAction::triggered,
            pane->editor, &QPlainTextEdit::copy);
    connect(ui->actionCut, &QAction::triggered,
            pane->editor, &QPlainTextEdit::cut);
    connect(ui->actionPaste, &QAction::triggered,
            pane->editor, &QPlainTextEdit::paste);

    connect(ui->actionShapes, &QAction::triggered,
            this, &ScriptWindow::openShapesLibrary);

    graph->installWatcher(this);

    setCentralWidget(pane);
    resize(600, 800);
    show();
}

ScriptWindow::~ScriptWindow()
{
    if (graph)
        graph->uninstallWatcher(this);
}

void ScriptWindow::trigger(const GraphState& state)
{
    if (state.nodes.count(pane->node) == 0)
    {
        pane->node = NULL;
        close();
    }
}

void ScriptWindow::openShapesLibrary() const
{
    PyObject* fab_mod = PyImport_ImportModule("fab");
    PyObject* shapes_mod = PyObject_GetAttrString(fab_mod, "shapes");
    PyObject* shapes_path = PyObject_GetAttrString(shapes_mod, "__file__");

    Q_ASSERT(!PyErr_Occurred());

    auto filepath = QString::fromUtf8(PyUnicode_AsUTF8(shapes_path));

    QFile shapes(filepath);
    if (shapes.open(QIODevice::ReadOnly))
    {
        QTextStream in(&shapes);
        QString txt = in.readAll();

        auto ed = new QPlainTextEdit();
        ScriptEditor::styleEditor(ed);
        ed->document()->setPlainText(txt);
        ed->setReadOnly(true);
        ed->moveCursor(QTextCursor::Start);
        ed->resize(600, 600);
        ed->show();
    }

    Py_DECREF(fab_mod);
    Py_DECREF(shapes_mod);
    Py_DECREF(shapes_path);
}

