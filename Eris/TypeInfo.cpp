TypeInfo::TypeInfo(const Atlas::Objects::Operation::Info &info)
{
	
}

void TypeInfo::addParent(TypeInfoPtr tp)
{
	assert(tp);
	
	if (_parents.count(tp)) {
		cerr << "duplicate type info set" << endl;
		// it's critcial we bail fast here to avoid infitite mutual recursion with addChild
		return;
	}
	
	if (_ancestors.count(tp)) {
		throw InvalidOperation("Bad inheritance graph : new parent is ancestor");
	}
	
	// update the gear
	_parents.insert(tp);
	_ancestors.insert(tp);
	
	_ancestors.insert( tp->getAncestors()  );
	
	// note this will never recurse deep becuase of the fast exiting up top
	tp->addChild(this);
}

void TypeInfo::addChild(TypeInfoPtr tp)
{
	assert(tp);
	
	if (_children.count(tp)) {
		cerr << "duplicate child node set" << endl;
		return; // same need for early bail-out here
	}
	
	_children.insert(tp);
	_descendants.insert(tp);
	
	// again this will not recurse due to the termination code
	tp->addParent(this);
}