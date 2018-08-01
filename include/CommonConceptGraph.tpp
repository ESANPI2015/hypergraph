#include <queue>
#include <set>
#include <vector>
#include <iostream>

template< typename MatchFunc, typename ResourceFunc, typename CostFunc > CommonConceptGraph CommonConceptGraph::mapIt (MatchFunc m, ResourceFunc r, CostFunc c, const UniqueId& relUid)
{
    CommonConceptGraph result(*this);

    // We have to cycle through all pairs of concepts
    Hyperedges all(result.find());
    std::set< UniqueId > toBeMapped;
    std::set< UniqueId > toBeMappedTo;

    // First step: Filter out non-matching pairs & gather available ressources
    std::map< UniqueId, float> ressources;
    for (const UniqueId& a : all)
    {
        for (const UniqueId& b : all)
        {
	        // If there is a valid match, register it
    	    if (m(result, a, b))
    	    {
		        toBeMapped.insert(a);
		        toBeMappedTo.insert(b);
		        // TODO2: What if we have multiple/different ressources?
		        ressources[b] = r(result, b);
    	    }
        }
    }
     

    // As long as we have mappable concepts, we try to greedily find a (sub-)optimal mapping
    auto cmp = [] (std::pair< float, std::pair< UniqueId, UniqueId > > a, std::pair< float, std::pair< UniqueId, UniqueId > > b) -> bool { return (a.first < b.first);};
    while (toBeMapped.size() > 0)
    {
        std::priority_queue< std::pair< float, std::pair< UniqueId, UniqueId > >, std::vector< std::pair< float, std::pair< UniqueId, UniqueId > > >,  decltype(cmp) > q(cmp);
        for (const UniqueId& a : toBeMapped)
        {
            for (const UniqueId& b : toBeMappedTo)
            {
                // For each pair, we have to decide if they COULD be mapped
                if (m(result, a, b))
                {
                    // If they COULD be mapped, we have to calculate costs
                    const float costs(c(result, a, b));

		            // The costs consume ressources. So we have subtract the costs from the available ressources
		            const float ressourcesLeft(ressources[b] - costs);
                    std::cout << "\t\t" << result.get(a)->label() << " -> " << result.get(b)->label() << ": " << ressourcesLeft << std::endl;

                    // Now the mapping and its associated cost has to be inserted into a priority queue
		            // NOTE: The mapping which consumes LESS ressources is at the top of the queue!
                    q.push({ressourcesLeft, {a,b}});
                }
            }
        }

        // Here we have a priority queue assembled with all possible mappings, where the top element should be the mapping with LOWEST cost aka the HIGHEST amount of ressources left
        // We take it and map the two concepts
        if (!q.empty())
        {
            std::pair< float, std::pair< UniqueId, UniqueId > > best(q.top());
            std::cout << "\t" << result.get(best.second.first)->label() << " -> " << result.get(best.second.second)->label() << ": " << best.first << std::endl;

            result.factFrom(Hyperedges{best.second.first}, Hyperedges{best.second.second}, relUid);
	        ressources[best.second.second] -= c(result, best.second.first, best.second.second);

	        // Remove only the first entity from 
	        toBeMapped.erase(best.second.first);
        } else {
	        break;
	    }
    }

    return result;
}
