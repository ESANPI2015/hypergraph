template <typename T> bool Set::kindOf()
{
    Relation *subclassOf = isA();
    if (subclassOf->pointingTo(T::Superclass()->label()).size())
        return true;
    else
        return false;
}

template <typename T> T* Set::create(const std::string& label)
{
    Set* neu = Set::create(label);
    return neu->promote<T>();
}

template <typename T> T* Set::promote()
{
    T* casted = static_cast< T* >(this);
    this->isA(T::Superclass());
    return casted;
}
