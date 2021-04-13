/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "outputgen.h"

class HtmlCodeGenerator : public CodeOutputInterface
{
  public:
    HtmlCodeGenerator(TextStream &t,const QCString &relPath);
    HtmlCodeGenerator(TextStream &t);
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    void setRelativePath(const QCString &path);
    void codify(const char *text);
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip);
    void writeTooltip(const char *id,
                      const DocLinkInfo &docInfo,
                      const char *decl,
                      const char *desc,
                      const SourceLinkInfo &defInfo,
                      const SourceLinkInfo &declInfo
                     );
    void writeLineNumber(const char *,const char *,const char *,int);
    void startCodeLine(bool);
    void endCodeLine();
    void startFontClass(const char *s);
    void endFontClass();
    void writeCodeAnchor(const char *anchor);
    void setCurrentDoc(const Definition *,const char *,bool) {}
    void addWord(const char *,bool) {}
    void startCodeFragment(const char *style);
    void endCodeFragment(const char *);

  private:
    void _writeCodeLink(const char *className,
                        const char *ref,const char *file,
                        const char *anchor,const char *name,
                        const char *tooltip);
    void docify(const char *str);
    TextStream &m_t;
    int m_col = 0;
    QCString m_relPath;
    bool m_lineOpen = false;
    int m_id = 0;
};

/** Generator for HTML output */
class HtmlGenerator : public OutputGenerator
{
  public:
    HtmlGenerator();
    HtmlGenerator &operator=(const HtmlGenerator &g);
    HtmlGenerator(const HtmlGenerator &g);
    virtual ~HtmlGenerator();
    virtual std::unique_ptr<OutputGenerator> clone() const;

    virtual OutputType type() const { return Html; }
    static void init();
    static void writeStyleSheetFile(TextStream &t);
    static void writeHeaderFile(TextStream &t, const char *cssname);
    static void writeFooterFile(TextStream &t);
    static void writeTabData();
    static void writeSearchInfo(TextStream &t,const QCString &relPath);
    static void writeSearchData(const char *dir);
    static void writeSearchPage();
    static void writeExternalSearchPage();
    static QCString writeLogoAsString(const char *path);
    static QCString writeSplitBarAsString(const char *name,const char *relpath);

    // ---- CodeOutputInterface
    void codify(const char *text)
    { m_codeGen.codify(text); }
    void writeCodeLink(const char *ref,const char *file,
                       const char *anchor,const char *name,
                       const char *tooltip)
    { m_codeGen.writeCodeLink(ref,file,anchor,name,tooltip); }
    void writeLineNumber(const char *ref,const char *file,const char *anchor,int lineNumber)
    { m_codeGen.writeLineNumber(ref,file,anchor,lineNumber); }
    void writeTooltip(const char *id, const DocLinkInfo &docInfo, const char *decl,
                      const char *desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo
                     )
    { m_codeGen.writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }
    void startCodeLine(bool hasLineNumbers)
    { m_codeGen.startCodeLine(hasLineNumbers); }
    void endCodeLine()
    { m_codeGen.endCodeLine(); }
    void startFontClass(const char *s)
    { m_codeGen.startFontClass(s); }
    void endFontClass()
    { m_codeGen.endFontClass(); }
    void writeCodeAnchor(const char *anchor)
    { m_codeGen.writeCodeAnchor(anchor); }
    void startCodeFragment(const char *style)
    { m_codeGen.startCodeFragment(style); }
    void endCodeFragment(const char *style)
    { m_codeGen.endCodeFragment(style); }
    // ---------------------------

    void setCurrentDoc(const Definition *context,const char *anchor,bool isSourceFile);
    void addWord(const char *word,bool hiPriority);
    void writeDoc(DocNode *,const Definition *,const MemberDef *,int id);

    void startFile(const char *name,const char *manName,const char *title,int id);
    void writeFooter(const char *navPath);
    void endFile();
    void clearBuffer();
    void writeSearchInfo();

    void startIndexSection(IndexSections) {}
    void endIndexSection(IndexSections) {}
    void writePageLink(const char *,bool) {}
    void startProjectNumber();
    void endProjectNumber();
    void writeStyleInfo(int part);
    void startTitleHead(const char *);
    void endTitleHead(const char *,const char *);
    void startTitle() { m_t << "<div class=\"title\">"; }
    void endTitle() { m_t << "</div>"; }

    void startParagraph(const char *classDef);
    void endParagraph();
    void writeString(const char *text);
    void startIndexListItem();
    void endIndexListItem();
    void startIndexList();
    void endIndexList();
    void startIndexKey();
    void endIndexKey();
    void startIndexValue(bool);
    void endIndexValue(const char *,bool);
    void startItemList()  { m_t << "<ul>\n"; }
    void endItemList()    { m_t << "</ul>\n"; }
    void startIndexItem(const char *ref,const char *file);
    void endIndexItem(const char *ref,const char *file);
    void docify(const char *text);

    void writeObjectLink(const char *ref,const char *file,
                         const char *anchor,const char *name);

    void startTextLink(const char *file,const char *anchor);
    void endTextLink();
    void startHtmlLink(URLString url);
    void endHtmlLink();
    void startTypewriter() { m_t << "<code>"; }
    void endTypewriter()   { m_t << "</code>"; }
    void startGroupHeader(int);
    void endGroupHeader(int);
    void startItemListItem() { m_t << "<li>"; }
    void endItemListItem() { m_t << "</li>\n"; }

    void startMemberSections();
    void endMemberSections();
    void startHeaderSection();
    void endHeaderSection();
    void startMemberHeader(const char *, int);
    void endMemberHeader();
    void startMemberSubtitle();
    void endMemberSubtitle();
    void startMemberDocList();
    void endMemberDocList();
    void startMemberList();
    void endMemberList();
    void startInlineHeader();
    void endInlineHeader();
    void startAnonTypeScope(int) {}
    void endAnonTypeScope(int) {}
    void startMemberItem(const char *anchor,int,const char *inheritId);
    void endMemberItem();
    void startMemberTemplateParams();
    void endMemberTemplateParams(const char *anchor,const char *inheritId);
    void startCompoundTemplateParams();
    void endCompoundTemplateParams();

    void startMemberGroupHeader(bool);
    void endMemberGroupHeader();
    void startMemberGroupDocs();
    void endMemberGroupDocs();
    void startMemberGroup();
    void endMemberGroup(bool);

    void insertMemberAlign(bool);
    void insertMemberAlignLeft(int,bool);
    void startMemberDescription(const char *anchor,const char *inheritId, bool typ);
    void endMemberDescription();
    void startMemberDeclaration() {}
    void endMemberDeclaration(const char *anchor,const char *inheritId);
    void writeInheritedSectionTitle(const char *id,   const char *ref,
                                    const char *file, const char *anchor,
                                    const char *title,const char *name);

    void writeRuler()    { m_t << "<hr/>"; }
    void writeAnchor(const char *,const char *name)
                         { m_t << "<a name=\"" << name <<"\" id=\"" << name << "\"></a>"; }
    void startEmphasis() { m_t << "<em>";  }
    void endEmphasis()   { m_t << "</em>"; }
    void startBold()     { m_t << "<b>"; }
    void endBold()       { m_t << "</b>"; }
    void startDescription() { m_t << "\n<dl>\n"; }
    void endDescription()   { m_t << "\n</dl>\n\n"; }
    void startDescItem()    { m_t << "<dt>"; }
    void endDescItem()      { m_t << "</dt>"; }
    void startDescForItem() { m_t << "<dd>"; }
    void endDescForItem()   { m_t << "</dd>\n"; }
    void lineBreak(const char *style);
    void writeChar(char c);
    void startMemberDoc(const char *clName, const char *memName,
                        const char *anchor, const char *title,
                        int memCount, int memTotal, bool showInline);
    void endMemberDoc(bool);
    void startDoxyAnchor(const char *fName,const char *manName,
                         const char *anchor,const char *name,
                         const char *args);
    void endDoxyAnchor(const char *fName,const char *anchor);
    void writeLatexSpacing() {}
    void writeStartAnnoItem(const char *type,const char *file,
                            const char *path,const char *name);
    void writeEndAnnoItem(const char *) { m_t << "\n"; }
    void startSubsection()    { m_t << "<h2>"; }
    void endSubsection()      { m_t << "</h2>\n"; }
    void startSubsubsection() { m_t << "<h3>"; }
    void endSubsubsection()   { m_t << "</h3>\n"; }
    void startCenter()        { m_t << "<center>\n"; }
    void endCenter()          { m_t << "</center>\n"; }
    void startSmall()         { m_t << "<small>\n"; }
    void endSmall()           { m_t << "</small>\n"; }
    void startExamples();
    void endExamples();
    void startParamList(ParamListTypes,const char *);
    void endParamList();
    void startSection(const char *,const char *,SectionType);
    void endSection(const char *,SectionType);
    void addIndexItem(const char *,const char *);
    void startIndent();
    void endIndent();
    void writeSynopsis() {}
    void startClassDiagram();
    void endClassDiagram(const ClassDiagram &,const char *,const char *);
    void startPageRef() {}
    void endPageRef(const char *,const char *) {}
    void startQuickIndices() {}
    void endQuickIndices();
    void writeSplitBar(const char *name);
    void writeNavigationPath(const char *s);
    void writeLogo();
    void writeQuickLinks(bool compact,HighlightedItem hli,const char *file);
    void writeSummaryLink(const char *file,const char *anchor,const char *title,bool first);
    void startContents();
    void endContents();
    void startPageDoc(const char *pageTitle);
    void endPageDoc();
    void writeNonBreakableSpace(int);

    void startDescTable(const char *title);
    void endDescTable();
    void startDescTableRow();
    void endDescTableRow();
    void startDescTableTitle();
    void endDescTableTitle();
    void startDescTableData();
    void endDescTableData();

    void startDotGraph();
    void endDotGraph(DotClassGraph &g);
    void startInclDepGraph();
    void endInclDepGraph(DotInclDepGraph &g);
    void startGroupCollaboration();
    void endGroupCollaboration(DotGroupCollaboration &g);
    void startCallGraph();
    void endCallGraph(DotCallGraph &g);
    void startDirDepGraph();
    void endDirDepGraph(DotDirDeps &g);
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g);

    void startTextBlock(bool)
    { m_t << "<div class=\"textblock\">"; }
    void endTextBlock(bool)
    { m_t << "</div>"; }
    void lastIndexPage() {}

    void startMemberDocPrefixItem();
    void endMemberDocPrefixItem();
    void startMemberDocName(bool);
    void endMemberDocName();
    void startParameterType(bool first,const char *key);
    void endParameterType();
    void startParameterName(bool);
    void endParameterName(bool last,bool emptyList,bool closeBracket);
    void startParameterList(bool);
    void endParameterList();
    virtual void exceptionEntry(const char*,bool);

    void startConstraintList(const char *);
    void startConstraintParam();
    void endConstraintParam();
    void startConstraintType();
    void endConstraintType();
    void startConstraintDocs();
    void endConstraintDocs();
    void endConstraintList();

    void startMemberDocSimple(bool);
    void endMemberDocSimple(bool);
    void startInlineMemberType();
    void endInlineMemberType();
    void startInlineMemberName();
    void endInlineMemberName();
    void startInlineMemberDoc();
    void endInlineMemberDoc();

    void startLabels();
    void writeLabel(const char *l,bool isLast);
    void endLabels();

  private:
    static void writePageFooter(TextStream &t,const QCString &,const QCString &,const QCString &);
    QCString m_lastTitle;
    QCString m_lastFile;
    QCString m_relPath;
    void docify(const char *text,bool inHtmlComment);

    int m_sectionCount = 0;
    bool m_emptySection = false;
    HtmlCodeGenerator m_codeGen;
};

#endif
