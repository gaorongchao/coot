#ifndef COOT_LIGAND_EDITOR_CANVAS_TOOLS_HPP
#define COOT_LIGAND_EDITOR_CANVAS_TOOLS_HPP
#include "model.hpp"

namespace coot::ligand_editor_canvas {

namespace impl {
    /// This is a forward declaration of a structure defined at
    /// "core.hpp"
    struct WidgetCoreData;
    /// This is a forward declaration of a structure defined at
    /// "core.hpp"
    struct CootLigandEditorCanvasPriv;
}

class BondModifier {
    public:
    enum class BondModifierMode {
        Single,
        Double,
        Triple
    };
    private:

    BondModifierMode mode;
    public:
    BondModifier(BondModifierMode) noexcept;

    CanvasMolecule::BondType get_target_bond_type() const noexcept;

};

class ElementInsertion {
    public:
    enum class Element {
        C,
        N,
        O,
        S,
        P,
        H,
        F,
        Cl,
        Br,
        I
    };

    private:
    std::variant<Element,unsigned int> element;
    public:
    ElementInsertion(const char* symbol);
    ElementInsertion(Element el) noexcept;

    unsigned int get_atomic_number() const noexcept;
};



class StructureInsertion {
    public:

    enum class Structure: unsigned int {
        CycloPropaneRing,
        CycloButaneRing,
        CycloPentaneRing,
        CycloHexaneRing,
        BenzeneRing,
        CycloHeptaneRing,
        CycloOctaneRing,
        // todo:
        // "env residues"

    };
    private:
    Structure structure;

    public:
    StructureInsertion(Structure) noexcept;

    Structure get_structure() const noexcept;

};

class ChargeModifier {

};

class DeleteTool {

};

class MoveTool {
    std::optional<std::pair<int,int>> prev_move_pos;
    std::optional<std::pair<int,int>> current_move_pos;
    /// Describes whether the user is currently dragging with their mouse
    bool in_move;
    std::optional<unsigned int> canvas_mol_idx;

    public:

    void begin_move(int x, int y) noexcept;
    std::pair<int,int> end_move();
    void update_current_move_pos(int x, int y) noexcept;
    std::optional<std::pair<int,int>> get_current_offset() const;
    bool is_in_move() const noexcept;
    void set_canvas_molecule_index(unsigned int) noexcept;
    std::optional<unsigned int> get_canvas_molecule_index() const noexcept;
    MoveTool() noexcept;
};

class GeometryModifier {

};

class FormatTool {

};

class RotateTool {

};

class FlipTool {
    public:
    enum class FlipMode: unsigned char {
        /// Along the X axis
        Horizontal,
        /// Along the Y axis
        Vertical
    };
    private:
    FlipMode mode;
    public:
    FlipTool(FlipMode) noexcept;
    FlipMode get_mode() const noexcept;
};

class ActiveTool {
    public:
    enum class Variant: unsigned char {
        None,
        MoveTool,
        BondModifier,
        StructureInsertion,
        ElementInsertion,
        /// Stereo out
        GeometryModifier,
        DeleteHydrogens,
        Delete,
        Format,
        ChargeModifier,
        RotateTool,
        FlipTool
    };

    private:
    union {
        /// Valid for Variant::BondModifier
        BondModifier bond_modifier;
        /// Valid for Variant::ElementInsertion
        ElementInsertion element_insertion;
        /// Valid for Variant::StructureInsertion
        StructureInsertion structure_insertion;
        /// Valid for Variant::ChargeModifier
        ChargeModifier charge_modifier;
        /// Valid for Variant::Delete
        DeleteTool delete_tool;
        /// Valid for Variant::MoveTool
        MoveTool move_tool;
        /// Valid for Variant::GeometryModifier
        GeometryModifier geometry_modifier;
        /// Valid for Variant::Format
        FormatTool format_tool;
        /// Valid for Variant::RotateTool
        RotateTool rotate_tool;
        /// Valid for Variant::FlipTool
        FlipTool flip_tool;
    };
    Variant variant;
    /// Non-owning pointer
    impl::WidgetCoreData* widget_data;

    /// Checks if the internal variant (the kind of the tool) matches what's expected (passed as argument).
    /// Throws an exception in case of a mismatch.
    void check_variant(Variant) const;

    /// Wraps `RDKit::MolOps::sanitizeMol()`.
    /// Used for checking validity and reversing kekulization.
    /// Does nothing when nonsensical molecules are allowed.
    void sanitize_molecule(RDKit::RWMol&) const;

    public:
    ActiveTool() noexcept;
    ActiveTool(ElementInsertion insertion) noexcept;
    ActiveTool(BondModifier modifier) noexcept;
    ActiveTool(DeleteTool) noexcept;
    ActiveTool(ChargeModifier) noexcept;
    ActiveTool(MoveTool) noexcept;
    ActiveTool(StructureInsertion insertion) noexcept;
    ActiveTool(GeometryModifier modifier) noexcept;
    ActiveTool(FormatTool) noexcept;
    ActiveTool(RotateTool) noexcept;
    ActiveTool(FlipTool) noexcept;

    Variant get_variant() const noexcept;
    /// Valid for Variant::ElementInsertion.
    /// Inserts currently chosen atom at the given coordinates.
    void insert_atom(int x, int y);

    /// Valid for Variant::BondModifier.
    /// Changes the bond found at the given coordinates.
    /// The kind of the bond depends upon current BondModifierMode.
    void alter_bond(int x, int y);
    /// Valid for Variant::ChargeModifier.
    /// Modifies the charge of bond found at the given coordinates.
    void alter_charge(int x, int y);
    /// Valid for Variant::Delete.
    /// Deletes whatever is found at the given coordinates
    void delete_at(int x, int y);
    /// Valid for Variant::StructureInsertion.
    /// Inserts currently chosen structure at the given coordinates.
    void insert_structure(int x, int y);
    /// Valid for Variant::MoveTool
    /// Updates the current mouse coordinates
    /// allowing to compute adequate viewport translation.
    void update_move_cursor_pos(int x, int y);
    /// Valid for Variant::RotateTool
    /// Updates the current mouse coordinates
    /// allowing to compute adequate viewport rotation.
    void update_rotate_cursor_pos(int x, int y, bool snap_to_angle);
    /// Valid for Variant::MoveTool
    /// Ends move
    void end_move();
    /// Valid for Variant::RotateTool
    /// Ends rotation
    void end_rotate();
    /// Valid for Variant::MoveTool
    /// Begins move
    void begin_move(int x, int y);
    /// Valid for Variant::RotateTool
    /// Begins rotation
    void begin_rotate(int x, int y);
    /// Valid for Variant::MoveTool
    /// Returns if the user is currently dragging their mouse
    /// to shift the viewport.
    bool is_in_move() const;
    /// Valid for Variant::GeometryModifier.
    /// Changes geometry of the bond found at the given coordinates.
    void alter_geometry(int x, int y);
    /// Valid for Variant::Format.
    /// Forces re-computation of molecule geometry from scratch using RDKit.
    void format_at(int x, int y);
    /// Valid for Variant::FlipTool.
    /// Flip molecule under cursor around X/Y axis.
    void flip(int x, int y);

    /// Only meant to be invoked from within CootLigandEditorCanvas implementation
    ///
    /// Sets the pointer to access widget's data.
    void set_core_widget_data(impl::CootLigandEditorCanvasPriv* owning_widget) noexcept;
};

}

#endif // COOT_LIGAND_EDITOR_CANVAS_TOOLS_HPP