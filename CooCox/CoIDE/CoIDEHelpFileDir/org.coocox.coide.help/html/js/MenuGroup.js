/* 
Replicates the MenuGroup functionality in dropdown lists because ASP.NET doesn't support these. All 
elements with value = '0' are set as MenuGroup items and are not selectable
*/

// Stops the user selecting a disabled entry
function StopDisableSelect(selectList)
{
	var found = false;
	if(selectList.options[selectList.selectedIndex].disabled)
	{
		// Find first non-disabled entry after current entry
		for (var i = selectList.selectedIndex+1; i < selectList.options.length; i++)
		{
			if (!selectList.options[i].disabled)
			{
				selectList.selectedIndex = i;
				return;
			}
		}

		// No disabled found - so start from the beginning of the list and try again
		for (var i = 0; i < select.selectedIndex; i++)
		{
			if (!selectList.options[i].disabled)
			{
				selectList.selectedIndex = i;
				return;
			}
		}
	}
}	

function DisableMenuGroupElms(selectList)
{
	if (!selectList || !selectList.options || selectList.options.length == 0) return;
	for (var i = 0; i < selectList.options.length; i++)
	{
		if (selectList.options[i] && selectList.options[i].value == '0')
		{
			selectList.options[i].className = 'MenuGroup';
			selectList.options[i].disabled  = true;
			selectList.options[i].state     = 'disabled';
			selectList.options[i].style.paddingLeft = 0;
		}
		else
		{
			selectList.options[i].className = 'OptionInGroup';
		}
	}
	
	StopDisableSelect(selectList);
}