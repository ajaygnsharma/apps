function Dis(val)
{
  var i1 = document.getElementById('i1');
  var i2 = document.getElementById('i2');
  var i3 = document.getElementById('i3');
  var i4 = document.getElementById('i4');
  var i5 = document.getElementById('i5');
  var i6 = document.getElementById('i6');
  var o1 = document.getElementById('o1');
  var o2 = document.getElementById('o2');
  var o3 = document.getElementById('o3');
  var o4 = document.getElementById('o4');
  var o5 = document.getElementById('o5');
  var o6 = document.getElementById('o6');
  var p1ch = document.getElementById('p1ch');
  var p2ch = document.getElementById('p2ch');
  var p3ch = document.getElementById('p3ch');
  var p4ch = document.getElementById('p4ch');
  var p5ch = document.getElementById('p5ch');
  var p6ch = document.getElementById('p6ch');
  var c_ii1 = document.getElementById("c_ii1");
  var c_ii2 = document.getElementById("c_ii2");
  var c_ii3 = document.getElementById("c_ii3");
  var c_io1 = document.getElementById("c_io1");
  var c_io2 = document.getElementById("c_io2");
  var c_io3 = document.getElementById("c_io3");
  var c_ti1 = document.getElementById("c_ti1");
  var c_ti2 = document.getElementById("c_ti2");
  var c_ti3 = document.getElementById("c_ti3");
  var c_to1 = document.getElementById("c_to1");
  var c_to2 = document.getElementById("c_to2");
  var c_to3 = document.getElementById("c_to3");
	  
  if(val == "1")
  {
    i1.disabled = false;
	i2.disabled = false;
	i3.disabled = false;
	i4.disabled = false;
	i5.disabled = false;
	i6.disabled = false;
	o1.disabled = false;
	o2.disabled = false;
	o3.disabled = false;
	o4.disabled = false;
	o5.disabled = false;
	o6.disabled = false;
	p1ch.disabled = false;
	p2ch.disabled = false;
	p3ch.disabled = false;
	p4ch.disabled = false;
	p5ch.disabled = false;
	p6ch.disabled = false;
	p1cl.disabled = false;
	p2cl.disabled = false;
	p3cl.disabled = false;
	p4cl.disabled = false;
	p5cl.disabled = false;
	p6cl.disabled = false;
	c_ii1.disabled = true;
	c_ii2.disabled = true;
	c_ii3.disabled = true;
	c_io1.disabled = true;
	c_io2.disabled = true;
	c_io3.disabled = true;
	c_ti1.disabled = true;
	c_ti2.disabled = true;
	c_ti3.disabled = true;
	c_to1.disabled = true;
	c_to2.disabled = true;
	c_to3.disabled = true;
  }
  else if(val == "2")
  {
    i1.disabled = true;
	i2.disabled = true;
	i3.disabled = true;
	i4.disabled = true;
	i5.disabled = true;
	i6.disabled = true;
	o1.disabled = true;
	o2.disabled = true;
	o3.disabled = true;
	o4.disabled = true;
	o5.disabled = true;
	o6.disabled = true;
	p1ch.disabled = true;
	p2ch.disabled = true;
	p3ch.disabled = true;
	p4ch.disabled = true;
	p5ch.disabled = true;
	p6ch.disabled = true;
	p1cl.disabled = true;
	p2cl.disabled = true;
	p3cl.disabled = true;
	p4cl.disabled = true;
	p5cl.disabled = true;
	p6cl.disabled = true;
	
	c_ii1.disabled = false;
	c_ii2.disabled = false;
	c_ii3.disabled = false;
	c_io1.disabled = false;
	c_io2.disabled = false;
	c_io3.disabled = false;
	c_ti1.disabled = false;
	c_ti2.disabled = false;
	c_ti3.disabled = false;
	c_to1.disabled = false;
	c_to2.disabled = false;
	c_to3.disabled = false;
  }
}



