/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "GenTemplateInstance.h"
#include "Overview.h"

#include <CppControl.h>
#include <Scope.h>
#include <Names.h>
#include <Symbols.h>
#include <CoreTypes.h>
#include <Literals.h>

#include <QtCore/QVarLengthArray>
#include <QtCore/QDebug>

using namespace CPlusPlus;

namespace {

class ApplySubstitution
{
public:
    ApplySubstitution(const LookupContext &context, Symbol *symbol, const GenTemplateInstance::Substitution &substitution);
    ~ApplySubstitution();

    Control *control() const { return context.control(); }

    FullySpecifiedType apply(const Name *name);
    FullySpecifiedType apply(const FullySpecifiedType &type);

    int findSubstitution(const Identifier *id) const;
    FullySpecifiedType applySubstitution(int index) const;

private:
    class ApplyToType: protected TypeVisitor
    {
    public:
        ApplyToType(ApplySubstitution *q)
            : q(q) {}

        FullySpecifiedType operator()(const FullySpecifiedType &ty)
        {
            FullySpecifiedType previousType = switchType(ty);
            accept(ty.type());
            return switchType(previousType);
        }

    protected:
        using TypeVisitor::visit;

        Control *control() const
        { return q->control(); }

        FullySpecifiedType switchType(const FullySpecifiedType &type)
        {
            FullySpecifiedType previousType = _type;
            _type = type;
            return previousType;
        }

        virtual void visit(VoidType *)
        {
            // nothing to do
        }

        virtual void visit(IntegerType *)
        {
            // nothing to do
        }

        virtual void visit(FloatType *)
        {
            // nothing to do
        }

        virtual void visit(PointerToMemberType *)
        {
            qDebug() << Q_FUNC_INFO; // ### TODO
        }

        virtual void visit(PointerType *ptrTy)
        {
            _type.setType(control()->pointerType(q->apply(ptrTy->elementType())));
        }

        virtual void visit(ReferenceType *refTy)
        {
            _type.setType(control()->referenceType(q->apply(refTy->elementType())));
        }

        virtual void visit(ArrayType *arrayTy)
        {
            _type.setType(control()->arrayType(q->apply(arrayTy->elementType()), arrayTy->size()));
        }

        virtual void visit(NamedType *ty)
        {
            FullySpecifiedType n = q->apply(ty->name());
            _type.setType(n.type());
        }

        virtual void visit(Function *funTy)
        {
            Function *fun = control()->newFunction(/*sourceLocation=*/ 0, funTy->name());
            fun->setScope(funTy->scope());
            fun->setConst(funTy->isConst());
            fun->setVolatile(funTy->isVolatile());
            fun->setVirtual(funTy->isVirtual());
            fun->setAmbiguous(funTy->isAmbiguous());
            fun->setVariadic(funTy->isVariadic());

            fun->setReturnType(q->apply(funTy->returnType()));

            for (unsigned i = 0; i < funTy->argumentCount(); ++i) {
                Argument *originalArgument = funTy->argumentAt(i)->asArgument();
                Argument *arg = control()->newArgument(/*sourceLocation*/ 0,
                                                       originalArgument->name());

                arg->setType(q->apply(originalArgument->type()));
                arg->setInitializer(originalArgument->initializer());
                fun->arguments()->enterSymbol(arg);
            }

            _type.setType(fun);
        }

        virtual void visit(Namespace *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(Class *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(Enum *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(ForwardClassDeclaration *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(ObjCClass *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(ObjCProtocol *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(ObjCMethod *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(ObjCForwardClassDeclaration *)
        {
            qDebug() << Q_FUNC_INFO;
        }

        virtual void visit(ObjCForwardProtocolDeclaration *)
        {
            qDebug() << Q_FUNC_INFO;
        }

    private:
        ApplySubstitution *q;
        FullySpecifiedType _type;
        QHash<Symbol *, FullySpecifiedType> _processed;
    };

    class ApplyToName: protected NameVisitor
    {
    public:
        ApplyToName(ApplySubstitution *q): q(q) {}

        FullySpecifiedType operator()(const Name *name)
        {
            FullySpecifiedType previousType = switchType(FullySpecifiedType());
            accept(name);
            return switchType(previousType);
        }

    protected:
        Control *control() const
        { return q->control(); }

        int findSubstitution(const Identifier *id) const
        { return q->findSubstitution(id); }

        FullySpecifiedType applySubstitution(int index) const
        { return q->applySubstitution(index); }

        FullySpecifiedType switchType(const FullySpecifiedType &type)
        {
            FullySpecifiedType previousType = _type;
            _type = type;
            return previousType;
        }

        virtual void visit(const NameId *name)
        {
            int index = findSubstitution(name->identifier());

            if (index != -1)
                _type = applySubstitution(index);

            else
                _type = control()->namedType(name);
        }

        virtual void visit(const TemplateNameId *name)
        {
            QVarLengthArray<FullySpecifiedType, 8> arguments(name->templateArgumentCount());
            for (unsigned i = 0; i < name->templateArgumentCount(); ++i) {
                FullySpecifiedType argTy = name->templateArgumentAt(i);
                arguments[i] = q->apply(argTy);
            }

            const TemplateNameId *templId = control()->templateNameId(name->identifier(),
                                                                      arguments.data(),
                                                                      arguments.size());
            _type = control()->namedType(templId);
        }

        virtual void visit(const QualifiedNameId *name)
        {
            QVarLengthArray<const Name *, 8> names(name->nameCount());
            for (unsigned i = 0; i < name->nameCount(); ++i) {
                const Name *n = name->nameAt(i);

                if (const TemplateNameId *templId = n->asTemplateNameId()) {
                    QVarLengthArray<FullySpecifiedType, 8> arguments(templId->templateArgumentCount());
                    for (unsigned templateArgIndex = 0; templateArgIndex < templId->templateArgumentCount(); ++templateArgIndex) {
                        FullySpecifiedType argTy = templId->templateArgumentAt(templateArgIndex);
                        arguments[templateArgIndex] = q->apply(argTy);
                    }

                    n = control()->templateNameId(templId->identifier(), arguments.data(), arguments.size());
                }

                names[i] = n;
            }

            const QualifiedNameId *q = control()->qualifiedNameId(names.data(), names.size(), name->isGlobal());
            _type = control()->namedType(q);
        }

        virtual void visit(const DestructorNameId *name)
        {
            Overview oo;
            qWarning() << "ignored name:" << oo(name);
        }

        virtual void visit(const OperatorNameId *name)
        {
            Overview oo;
            qWarning() << "ignored name:" << oo(name);
        }

        virtual void visit(const ConversionNameId *name)
        {
            Overview oo;
            qWarning() << "ignored name:" << oo(name);
        }

        virtual void visit(const SelectorNameId *name)
        {
            Overview oo;
            qWarning() << "ignored name:" << oo(name);
        }

    private:
        ApplySubstitution *q;
        FullySpecifiedType _type;
    };

public: // attributes
    LookupContext context;
    Symbol *symbol;
    GenTemplateInstance::Substitution substitution;
    ApplyToType applyToType;
    ApplyToName applyToName;
};

ApplySubstitution::ApplySubstitution(const LookupContext &context, Symbol *symbol,
                                     const GenTemplateInstance::Substitution &substitution)
    : context(context), symbol(symbol),
      substitution(substitution),
      applyToType(this), applyToName(this)
{ }

ApplySubstitution::~ApplySubstitution()
{
}

FullySpecifiedType ApplySubstitution::apply(const Name *name)
{
    FullySpecifiedType ty = applyToName(name);
    return ty;
}

FullySpecifiedType ApplySubstitution::apply(const FullySpecifiedType &type)
{
    FullySpecifiedType ty = applyToType(type);
    return ty;
}

int ApplySubstitution::findSubstitution(const Identifier *id) const
{
    Q_ASSERT(id != 0);

    for (int index = 0; index < substitution.size(); ++index) {
        QPair<const Identifier *, FullySpecifiedType> s = substitution.at(index);

        if (id->isEqualTo(s.first))
            return index;
    }

    return -1;
}

FullySpecifiedType ApplySubstitution::applySubstitution(int index) const
{
    Q_ASSERT(index != -1);
    Q_ASSERT(index < substitution.size());

    return substitution.at(index).second;
}

} // end of anonymous namespace

GenTemplateInstance::GenTemplateInstance(const LookupContext &context, const Substitution &substitution)
    : _symbol(0),
      _context(context),
      _substitution(substitution)
{ }

FullySpecifiedType GenTemplateInstance::operator()(Symbol *symbol)
{
    ApplySubstitution o(_context, symbol, _substitution);
    return o.apply(symbol->type());
}

Control *GenTemplateInstance::control() const
{ return _context.control(); }
