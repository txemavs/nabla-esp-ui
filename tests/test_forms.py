# Form schema and transaction tests; verifies validation and shared edit behavior.

import copy
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location("form_schema",ROOT/"external_components/nabla_navigation/form_schema.py")
schema=importlib.util.module_from_spec(spec)
spec.loader.exec_module(schema)

class FormsTests(unittest.TestCase):
    def fields(self):
        return [
            dict(key="level",label='Nivel "á"',type="number",min=0,max=100,step=5,initial=50),
            dict(key="mode",label="Mode",type="choice",options=["Auto","Manual","Eco"],initial=0),
            dict(key="enabled",label="Enabled",type="toggle",initial=True)]
    def test_reject_ambiguous_or_unbounded_metadata(self):
        cases=[[],self.fields()*3]
        for update in [
            {"step":0},{"initial":51},{"max":99},{"min":True},{"max":2**31},
            {"unknown":1},{"type":"script"},{"label":"bad\0name"}]:
            fields=self.fields();fields[0].update(update);cases.append(fields)
        for update in [{"options":["A","A"]},{"initial":3},{"options":[]},{"options":[2]},{"initial":False}]:
            fields=self.fields();fields[1].update(update);cases.append(fields)
        fields=self.fields();fields[2]["initial"]=1;cases.append(fields)
        fields=self.fields();fields[1]["key"]="level";cases.append(fields)
        for fields in cases:
            with self.subTest(fields=fields),self.assertRaises(ValueError):
                schema.validate(fields)
        self.assertEqual(schema.validate(self.fields()),self.fields())

    def test_transaction_input_equivalence_and_focus_restoration(self):
        code='#include "forms.h"\n#include <cassert>\n#include <climits>\n'+schema.emit(self.fields())
        code+=r"""
using namespace nabla_forms;
const char *words[]={"Demo","Save","Cancel","Accept","Confirm","Saved","Invalid","Yes","No","Back"};
Controller create(){Controller c;c.words=words;c.session.configure(fields,field_count);c.begin();return c;}
int main(){
 auto c=create();
 // Moving alone never edits. Cancelling numeric edit restores both focus and value.
 c.move(1);c.move(-1);assert(c.session.saved[0]==50);
 c.activate();c.adjust(INT_MAX);assert(c.value==100);
 c.adjust(INT_MIN);assert(c.value==0);c.back();
 assert(c.view==FormView::BROWSE && c.focus==0 && c.session.draft[0]==50);
 // U/D/Enter: increment, accept field, toggle, confirm form.
 c.activate();c.activate();c.move(1);c.activate();
 assert(c.session.draft[0]==55 && c.session.saved[0]==50);
 c.move(2);c.activate();assert(c.session.draft[2]==0);
 c.move(1);c.activate();assert(c.view==FormView::CONFIRM && c.focus==1);
 c.activate();assert(c.view==FormView::BROWSE && c.focus==3 && c.session.commits==0);
 c.activate();c.move(-1);c.activate();
 assert(c.view==FormView::SAVED && c.session.commits==1 && c.session.saved[0]==55 && c.session.saved[2]==0);
 assert(!c.session.commit());assert(c.activate());assert(c.session.commits==1);
 // Touch choices use the same controller actions and yield the same retained data.
 auto t=create();t.focus=0;t.activate();t.focus=1;t.activate();t.focus=2;t.activate();
 t.focus=2;t.activate();t.focus=3;t.activate();t.focus=0;t.activate();
 assert(t.session.saved==c.session.saved && t.session.commits==1);
 // Re-entering loads committed state; cancelling an entire form discards accepted fields.
 c.begin();c.focus=1;c.activate();c.move(2);c.activate();
 assert(c.session.draft[1]==2 && c.session.saved[1]==0);
 c.focus=4;assert(c.activate());c.begin();assert(c.session.draft[1]==0);
 // Choice cancel is reachable without ESC, including the final option.
 c.focus=1;c.activate();c.move(2);c.adjust(-1);assert(c.focus==1 && c.value==1);c.move(2);c.activate();
 assert(c.view==FormView::BROWSE && c.focus==1 && c.session.draft[1]==0);
 // Invalid drafts cannot be saved; closing validation restores Save.
 c.session.draft[0]=101;c.focus=3;c.activate();c.focus=0;c.activate();
 assert(c.view==FormView::INVALID && c.session.commits==1);
 c.activate();assert(c.focus==3);c.back();assert(c.session.saved[0]==55);
 // A transaction never crosses an int32 range during stepping.
 Field extreme={"edge","Edge",Kind::NUMBER,INT_MIN,INT_MAX,1,0,{}};
 auto e=create();e.session.configure(&extreme,1);e.begin();e.activate();
 e.adjust(INT_MAX);e.adjust(INT_MAX);assert(e.value==INT_MAX);
 e.adjust(INT_MIN);e.adjust(INT_MIN);assert(e.value==INT_MIN);
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            cpp=Path(tmp)/"forms.cpp";exe=Path(tmp)/"forms";cpp.write_text(code)
            subprocess.run(["g++","-std=c++17","-fsanitize=undefined,address",
                            "-I",str(ROOT/"external_components/nabla_navigation"),
                            str(cpp),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
